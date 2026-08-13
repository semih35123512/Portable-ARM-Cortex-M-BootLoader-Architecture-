/**
 * @file boot_storage_hc32f460_impl.c
 * @brief HC32 reference hooks — boot_meta only (no product zone FS).
 */
#include "boot_mcu_select.h"
#if defined(BOOT_MCU_HC32F460)

#include "boot_hooks.h"
#include "boot_meta.h"
#include "boot_config.h"
#include <stddef.h>

static boot_fw_slot_t s_hc32_fw_slot = BOOT_FW_SLOT_CANDIDATE;

void boot_hook_set_fw_slot(boot_fw_slot_t slot)
{
    s_hc32_fw_slot = slot;
}

boot_fw_slot_t boot_hook_get_fw_slot(void)
{
    return s_hc32_fw_slot;
}

bool boot_hook_get_firmware_start_node(uint16_t *node)
{
    boot_meta_t meta;

    if(node == NULL)
    {
        return false;
    }

    if(boot_meta_read(&meta) && boot_meta_is_valid(&meta))
    {
        if(s_hc32_fw_slot == BOOT_FW_SLOT_GOOD)
        {
            *node = meta.good_start_node;
        }
        else
        {
            *node = meta.cand_start_node;
        }
        if((*node != 0U) && (*node != 0xFFFFU))
        {
            return true;
        }
    }

#if (BOOT_SPI_FW_MODE == BOOT_SPI_FW_MODE_FIXED)
    if(s_hc32_fw_slot == BOOT_FW_SLOT_GOOD)
    {
        *node = (uint16_t)BOOT_SPI_FW_GOOD_SECTOR_FIRST;
    }
    else
    {
        *node = (uint16_t)BOOT_SPI_FW_CAND_SECTOR_FIRST;
    }
    return true;
#else
    (void)node;
    return false;
#endif
}

uint32_t boot_hook_get_expected_image_crc(void)
{
    boot_meta_t meta;

    if(boot_meta_read(&meta) && boot_meta_is_valid(&meta))
    {
        if(s_hc32_fw_slot == BOOT_FW_SLOT_GOOD)
        {
            if(meta.good_crc != 0U)
            {
                return meta.good_crc;
            }
        }
        else if(meta.cand_crc != 0U)
        {
            return meta.cand_crc;
        }
    }
    return 0U;
}

void boot_hook_on_crc_calculated(uint32_t crc)
{
    (void)crc;
}

#endif
