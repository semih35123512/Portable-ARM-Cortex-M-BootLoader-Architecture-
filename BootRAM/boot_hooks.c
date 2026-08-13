/**
 * @file boot_hooks.c
 * @brief Weak default hooks (add this file to your IDE project when porting).
 */
#include "boot_hooks.h"
#include "boot_config.h"
#include "boot_meta.h"

#if defined(__GNUC__) || defined(__clang__)
#define BOOT_WEAK __attribute__((weak))
#elif defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
#define BOOT_WEAK __attribute__((weak))
#elif defined(__ICCARM__)
#pragma weak boot_hook_get_firmware_start_node
#pragma weak boot_hook_get_expected_image_crc
#pragma weak boot_hook_on_crc_calculated
#pragma weak boot_hook_set_fw_slot
#pragma weak boot_hook_get_fw_slot
#define BOOT_WEAK
#else
#define BOOT_WEAK
#endif

static boot_fw_slot_t s_boot_fw_slot = BOOT_FW_SLOT_CANDIDATE;

BOOT_WEAK void boot_hook_set_fw_slot(boot_fw_slot_t slot)
{
    s_boot_fw_slot = slot;
}

BOOT_WEAK boot_fw_slot_t boot_hook_get_fw_slot(void)
{
    return s_boot_fw_slot;
}

BOOT_WEAK bool boot_hook_get_firmware_start_node(uint16_t *node)
{
    boot_meta_t meta;

    if(node == NULL)
    {
        return false;
    }

#if (BOOT_FW_START_NODE != 0xFFFFU)
    *node = (uint16_t)BOOT_FW_START_NODE;
    return (*node != 0U) && (*node != 0xFFFFU);
#else
    if(boot_meta_read(&meta) && boot_meta_is_valid(&meta))
    {
        if(s_boot_fw_slot == BOOT_FW_SLOT_GOOD)
        {
            *node = meta.good_start_node;
        }
        else
        {
            *node = meta.cand_start_node;
        }
        return (*node != 0U) && (*node != 0xFFFFU);
    }
    (void)node;
    return false;
#endif
}

BOOT_WEAK uint32_t boot_hook_get_expected_image_crc(void)
{
    boot_meta_t meta;
    if(boot_meta_read(&meta) && boot_meta_is_valid(&meta))
    {
        if(s_boot_fw_slot == BOOT_FW_SLOT_GOOD)
        {
            return meta.good_crc;
        }
        return meta.cand_crc;
    }
    return 0U;
}

BOOT_WEAK void boot_hook_on_crc_calculated(uint32_t crc)
{
    (void)crc;
}
