/**
 * @file boot_storage.c
 * @brief SPI firmware reader for IAP — MODE1 zone FS or MODE2 fixed sectors.
 */
#include "boot_mcu_select.h"
#include "boot_storage.h"
#include "boot_port.h"
#include "boot_board.h"
#include "boot_hooks.h"
#include "boot_config.h"
#include "boot_fw_slot.h"
#include <stddef.h>

#if !defined(BOOT_MCU_HC32F460) && !defined(BOOT_MCU_STM32_TEMPLATE) && \
    !defined(BOOT_MCU_STM32F4_GENERIC) && !defined(BOOT_MCU_USER)
#error "Unsupported BOOT_MCU_* selection."
#endif

#if (BOOT_SPI_FW_MODE == BOOT_SPI_FW_MODE_FIXED)

BOOT_RAM_FUNC static uint16_t boot_storage_fixed_first(boot_fw_slot_t slot)
{
    if(slot == BOOT_FW_SLOT_GOOD)
    {
        return (uint16_t)BOOT_SPI_FW_GOOD_SECTOR_FIRST;
    }
    return (uint16_t)BOOT_SPI_FW_CAND_SECTOR_FIRST;
}

BOOT_RAM_FUNC static uint16_t boot_storage_fixed_last(boot_fw_slot_t slot)
{
    if(slot == BOOT_FW_SLOT_GOOD)
    {
        return (uint16_t)BOOT_SPI_FW_GOOD_SECTOR_LAST;
    }
    return (uint16_t)BOOT_SPI_FW_CAND_SECTOR_LAST;
}

BOOT_RAM_FUNC bool boot_storage_get_first_firmware_node(uint16_t *node)
{
    uint16_t first;

    if(node == NULL)
    {
        return false;
    }

    /* Prefer meta/hook if it already stores the fixed window start; else config. */
#if (BOOT_FW_START_NODE != 0xFFFFU)
    *node = (uint16_t)BOOT_FW_START_NODE;
#else
    if(!boot_hook_get_firmware_start_node(node) ||
       (*node == 0U) || (*node == 0xFFFFU))
    {
        *node = boot_storage_fixed_first(boot_hook_get_fw_slot());
    }
#endif
    first = boot_storage_fixed_first(boot_hook_get_fw_slot());
    if((*node < first) || (*node > boot_storage_fixed_last(boot_hook_get_fw_slot())))
    {
        *node = first;
    }
    return (*node != 0U) && (*node != 0xFFFFU);
}

BOOT_RAM_FUNC bool boot_storage_read_next_node(uint16_t current_node, uint16_t *next_node)
{
    uint16_t last;

    if((next_node == NULL) || (current_node == 0U) || (current_node == 0xFFFFU))
    {
        return false;
    }

    last = boot_storage_fixed_last(boot_hook_get_fw_slot());
    if(current_node >= last)
    {
        return false;
    }
    *next_node = (uint16_t)(current_node + 1U);
    return true;
}

BOOT_RAM_FUNC bool boot_storage_read_node_payload(uint16_t node, uint32_t payload_offset, uint8_t *buffer, uint16_t size)
{
    const boot_port_cfg_t *cfg = boot_port_get_cfg();
    uint32_t read_address;
    uint16_t first;
    uint16_t last;

    if((buffer == NULL) || (size == 0U) || (node == 0U) || (node == 0xFFFFU))
    {
        return false;
    }

    first = boot_storage_fixed_first(boot_hook_get_fw_slot());
    last = boot_storage_fixed_last(boot_hook_get_fw_slot());
    if((node < first) || (node > last))
    {
        return false;
    }

    /* MODE2: raw contiguous binary — no zone header. */
    read_address = (cfg->sector_size * (uint32_t)(node - 1U)) + payload_offset;
    boot_board_storage_transfer_begin();
    boot_port_spi_read(buffer, read_address, size);
    boot_board_storage_transfer_end();
    return true;
}

#else /* BOOT_SPI_FW_MODE_ZONE — MODE1 */

BOOT_RAM_FUNC bool boot_storage_get_first_firmware_node(uint16_t *node)
{
    if(node == NULL)
    {
        return false;
    }

#if (BOOT_FW_START_NODE != 0xFFFFU)
    *node = (uint16_t)BOOT_FW_START_NODE;
    return (*node != 0U) && (*node != 0xFFFFU);
#else
    return boot_hook_get_firmware_start_node(node);
#endif
}

BOOT_RAM_FUNC bool boot_storage_read_next_node(uint16_t current_node, uint16_t *next_node)
{
    const boot_port_cfg_t *cfg = boot_port_get_cfg();
    uint32_t read_address;

    if((next_node == NULL) || (current_node == 0U) || (current_node == 0xFFFFU))
    {
        return false;
    }

    read_address = (cfg->sector_size * (current_node - 1U)) + cfg->next_node_offset;
    boot_board_storage_transfer_begin();
    boot_port_spi_read((uint8_t *)next_node, read_address, 2U);
    boot_board_storage_transfer_end();
    return (*next_node != 0U) && (*next_node != 0xFFFFU);
}

BOOT_RAM_FUNC bool boot_storage_read_node_payload(uint16_t node, uint32_t payload_offset, uint8_t *buffer, uint16_t size)
{
    const boot_port_cfg_t *cfg = boot_port_get_cfg();
    uint32_t read_address;

    if((buffer == NULL) || (size == 0U) || (node == 0U) || (node == 0xFFFFU))
    {
        return false;
    }

    read_address = (cfg->sector_size * (node - 1U)) + cfg->sector_header_size + payload_offset;
    boot_board_storage_transfer_begin();
    boot_port_spi_read(buffer, read_address, size);
    boot_board_storage_transfer_end();
    return true;
}

#endif /* BOOT_SPI_FW_MODE */
