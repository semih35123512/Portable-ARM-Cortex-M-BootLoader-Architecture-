#include "boot_mcu_select.h"
#if defined(BOOT_MCU_HC32F460)

#include "main.h"
#include "boot_port.h"
#include "boot_config.h"
#include "boot_ram_attr.h"
#include "boot_meta.h"
#include "boot_hc32f460_driver.h"
#include <string.h>
#include <stdbool.h>

static const boot_port_cfg_t g_boot_port_cfg = {
    .app_start_address = BOOT_APP_FLASH_START,
    .app_end_address = BOOT_APP_FLASH_END,
    .flash_page_size = BOOT_APP_FLASH_PAGE_SIZE,
    .transfer_size = BOOT_TRANSFER_SIZE,
    .sector_header_size = BOOT_SPI_SECTOR_HEADER_SIZE,
    .sector_size = BOOT_SPI_SECTOR_SIZE,
    .next_node_offset = BOOT_SPI_NEXT_NODE_OFFSET,
};

BOOT_RAM_FUNC void boot_port_disable_irq(void)
{
    __disable_irq();
}

BOOT_RAM_FUNC void boot_port_spi_config(void)
{
    boot_hc32f460_spi_config();
}

BOOT_RAM_FUNC void boot_port_spi_read(uint8_t *buffer, uint32_t address, uint16_t size)
{
    (void)boot_hc32f460_spi_read(buffer, address, size);
}

BOOT_RAM_FUNC void boot_port_set_erase_leds(void)
{
#ifndef DEBUG_ENABLE
    boot_hc32f460_led_on(RSSI_LED_5_Port, RSSI_LED_5_Pin);
    boot_hc32f460_led_off(RSSI_LED_4_Port, RSSI_LED_4_Pin);
#endif
    boot_hc32f460_led_on(RSSI_LED_3_Port, RSSI_LED_3_Pin);
    boot_hc32f460_led_off(RSSI_LED_2_Port, RSSI_LED_2_Pin);
    boot_hc32f460_led_on(RSSI_LED_1_Port, RSSI_LED_1_Pin);
    boot_hc32f460_led_off(RS485_LED_Port_1, RS485_LED_Pin_1);
    boot_hc32f460_led_on(ROLE2_LED_Port, ROLE2_LED_Pin);
    boot_hc32f460_led_off(ROLE1_LED_Port, ROLE1_LED_Pin);
    boot_hc32f460_led_on(RS485_LED_Port_2, RS485_LED_Pin_2);
    boot_hc32f460_led_off(LED_GSM_Port, LED_GSM_Pin);
    boot_hc32f460_led_on(LEDR_GPIO_Port, LEDR_Pin);
    boot_hc32f460_led_on(LEDB_GPIO_Port, LEDB_Pin);
    boot_hc32f460_led_off(CLIENT_LED_Port, CLIENT_LED_Pin);
    boot_hc32f460_led_on(SERVER_LED_Port, SERVER_LED_Pin);
}

BOOT_RAM_FUNC void boot_port_flash_erase_app_area(void)
{
    uint32_t pageError = 0;
    static FLASH_EraseInitTypeDef EraseInitStruct;

    boot_port_set_erase_leds();
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page = BOOT_APP_FLASH_START;
    EraseInitStruct.NbPages = (BOOT_APP_FLASH_END - BOOT_APP_FLASH_START) / BOOT_APP_FLASH_PAGE_SIZE;
    (void)boot_hc32f460_flash_erase(&EraseInitStruct, &pageError);
}

BOOT_RAM_FUNC void boot_port_flash_program_page(uint32_t flash_address, const uint32_t *data)
{
    (void)boot_hc32f460_flash_program(flash_address, (uint32_t)data);
}

BOOT_RAM_FUNC void boot_port_feed_watchdog(void)
{
#ifdef IWDG_ENABLE
    (void)boot_hc32f460_watchdog_feed();
#endif
}

BOOT_RAM_FUNC void boot_port_slider_lights(void)
{
    boot_hc32f460_slider_lights();
}

BOOT_RAM_FUNC void boot_port_crc_init(void)
{
    boot_hc32f460_crc_init();
}

BOOT_RAM_FUNC void boot_port_crc_accumulate32(const uint32_t *data, uint32_t word_count)
{
    (void)boot_hc32f460_crc_accumulate_data32(data, word_count);
}

BOOT_RAM_FUNC uint32_t boot_port_crc_get(void)
{
    return boot_hc32f460_crc_get();
}

BOOT_RAM_FUNC void boot_port_fail_and_reset(boot_fw_slot_t slot)
{
    boot_hc32f460_wipe_fw_slot_and_reset(slot);
}

bool boot_port_meta_erase_page(void)
{
    uint32_t pageError = 0;
    FLASH_EraseInitTypeDef eraseInit;

    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.Page = BOOT_META_FLASH_ADDR;
    eraseInit.NbPages = 1U;
    return (boot_hc32f460_flash_erase(&eraseInit, &pageError) == 0);
}

bool boot_port_meta_program(const uint8_t *data, uint32_t size)
{
    uint32_t word_buf[(sizeof(boot_meta_t) + 3U) / 4U];
    uint32_t i;

    if((data == NULL) || (size == 0U) || (size > sizeof(word_buf)))
    {
        return false;
    }

    for(i = 0U; i < (sizeof(word_buf) / 4U); i++)
    {
        word_buf[i] = 0xFFFFFFFFUL;
    }
    memcpy(word_buf, data, size);

    /* Program first flash page chunk at meta address (same path as IAP page program). */
    return (boot_hc32f460_flash_program(BOOT_META_FLASH_ADDR, (uint32_t)word_buf) == 0);
}

const boot_port_cfg_t *boot_port_get_cfg(void)
{
    return &g_boot_port_cfg;
}

#endif
