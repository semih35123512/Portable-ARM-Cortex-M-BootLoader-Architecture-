/**
 * @file boot_core.c
 * @brief MCU-independent IAP: SPI zone image -> CRC verify -> erase -> program (from RAM).
 */
#include "boot_core.h"
#include "boot_port.h"
#include "boot_storage.h"
#include "boot_board.h"
#include "boot_hooks.h"
#include "boot_config.h"
#include "boot_ram_attr.h"
#include "boot_meta.h"

#include <string.h>

BOOT_RAM_FUNC static void boot_core_accumulate_and_program(uint32_t *transfer_data,
                                                          uint16_t transfer_word_count,
                                                          uint32_t *flash_address,
                                                          uint8_t should_program)
{
    const boot_port_cfg_t *port_cfg = boot_port_get_cfg();
    boot_port_crc_accumulate32(transfer_data, transfer_word_count);
    boot_board_progress_tick();
    if(should_program > 0U)
    {
        boot_port_flash_program_page(*flash_address, transfer_data);
        *flash_address += port_cfg->flash_page_size;
    }
}

BOOT_RAM_FUNC void boot_core_firmware_update(uint32_t binary_file_size,
                                             uint16_t file_bin_start_point,
                                             boot_fw_slot_t slot)
{
    const boot_port_cfg_t *port_cfg = boot_port_get_cfg();
    uint8_t spi_data_crc_check_pass = 0U;
    uint8_t spi_data_crc_second_chance = 0U;
    uint32_t flash_address;
    uint16_t process_node;
    uint32_t remain_file_size;
    uint16_t read_size;
    uint32_t transfer_data[BOOT_TRANSFER_SIZE / 4U];
    uint16_t transfer_data_size;
    uint16_t remain_transfer_data_size;
    uint8_t buffer_stream[BOOT_STREAM_BUFFER_SIZE];
    uint32_t buffer_size;
    uint32_t crc_result;
    uint32_t expected_crc;

    boot_hook_set_fw_slot(slot);

    boot_port_disable_irq();
    boot_port_iap_gpio_init();
    boot_port_spi_config();

    while(1)
    {
        flash_address = port_cfg->app_start_address;
        transfer_data_size = 0U;
        remain_transfer_data_size = 0U;
        buffer_size = 0U;
        remain_file_size = binary_file_size;

        if(spi_data_crc_check_pass > 0U)
        {
            boot_board_set_erase_state();
            boot_port_flash_erase_app_area();
        }

        boot_port_crc_init();
        boot_hook_on_crc_calculated(0U);

        if(!boot_storage_get_first_firmware_node(&process_node))
        {
					  boot_meta_set_fail();
            boot_port_finish_and_reset(slot, 0U);
        }

        read_size = (uint16_t)(port_cfg->sector_size - file_bin_start_point);
        if(remain_file_size <= read_size)
        {
            read_size = (uint16_t)remain_file_size;
        }

        if(!boot_storage_read_node_payload(process_node, file_bin_start_point, buffer_stream, read_size))
        {
					  boot_meta_set_fail();
            boot_port_finish_and_reset(slot, 0U);
        }

        buffer_size += read_size;
        remain_file_size -= read_size;

        while(remain_file_size > 0U)
        {
            read_size = (uint16_t)port_cfg->sector_size;
            if(remain_file_size <= read_size)
            {
                read_size = (uint16_t)remain_file_size;
            }

            if(!boot_storage_read_next_node(process_node, &process_node))
            {
							  boot_meta_set_fail();
                boot_port_finish_and_reset(slot, 0U);
            }
            if(!boot_storage_read_node_payload(process_node, 0U, &buffer_stream[buffer_size], read_size))
            {
							 	boot_meta_set_fail();
                boot_port_finish_and_reset(slot, 0U);
            }

            buffer_size += read_size;
            remain_file_size -= read_size;

            while(buffer_size > port_cfg->transfer_size)
            {
                memcpy((uint8_t *)transfer_data, buffer_stream, port_cfg->transfer_size);
                transfer_data_size = (uint16_t)(port_cfg->transfer_size / 4U);

                memcpy(buffer_stream, &buffer_stream[port_cfg->transfer_size], (buffer_size - port_cfg->transfer_size));
                buffer_size -= port_cfg->transfer_size;

                boot_core_accumulate_and_program(transfer_data, transfer_data_size, &flash_address, spi_data_crc_check_pass);
            }

            boot_port_feed_watchdog();
        }

        memcpy((uint8_t *)transfer_data, buffer_stream, buffer_size);
        transfer_data_size = (uint16_t)(buffer_size / 4U);
        remain_transfer_data_size = (uint16_t)((port_cfg->transfer_size / 4U) - transfer_data_size);
        for(uint16_t i = 0U; i < remain_transfer_data_size; i++)
        {
            transfer_data[i + transfer_data_size] = 0xFFFFFFFFU;
        }

        boot_port_crc_accumulate32(transfer_data, transfer_data_size);
        if(spi_data_crc_check_pass > 0U)
        {
            boot_board_progress_tick();
            boot_port_flash_program_page(flash_address, transfer_data);
        }

        crc_result = boot_port_crc_get();
        boot_hook_on_crc_calculated(crc_result);
        expected_crc = boot_hook_get_expected_image_crc();

        if(spi_data_crc_check_pass == 0U)
        {
            if(expected_crc == crc_result)
            {
                spi_data_crc_check_pass = 1U;
            }
            else
            {
                spi_data_crc_second_chance++;
                if(spi_data_crc_second_chance > 1U)
                {
                    (void)boot_meta_set_fail();
                    boot_port_finish_and_reset(slot, 0U);
                }
            }
        }
        else
        {
            /* Second pass: CRC match after program = success latch (legacy semantics).
             * Mark PROGRAMMED then reset; stub waits for APP boot_ok.
             */
            if(expected_crc == crc_result)
            {
                (void)boot_meta_set_programmed();
                boot_port_finish_and_reset(slot, 1U);
            }
        }

        boot_port_feed_watchdog();
    }
}
