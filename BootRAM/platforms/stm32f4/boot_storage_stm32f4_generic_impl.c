/**
 * @file boot_storage_stm32f4_generic_impl.c
 * Zone walk is portable (boot_storage.c). Provide product hooks here if needed.
 */
#include "boot_mcu_select.h"
#if defined(BOOT_MCU_STM32F4_GENERIC)

#include "boot_hooks.h"
#include "boot_config.h"

static boot_fw_slot_t s_slot = BOOT_FW_SLOT_CANDIDATE;

void boot_hook_set_fw_slot(boot_fw_slot_t slot)
{
    s_slot = slot;
}

boot_fw_slot_t boot_hook_get_fw_slot(void)
{
    return s_slot;
}

/* If your product keeps OSOS zone tables, mirror hc32f460 storage hooks.
 * Default: use BOOT_FW_START_NODE from boot_config.h
 */
bool boot_hook_get_firmware_start_node(uint16_t *node)
{
    if(node == NULL)
    {
        return false;
    }
#if (BOOT_FW_START_NODE != 0xFFFFU)
    *node = (uint16_t)BOOT_FW_START_NODE;
    return (*node != 0U) && (*node != 0xFFFFU);
#else
    (void)node;
    return false;
#endif
}

uint32_t boot_hook_get_expected_image_crc(void)
{
    return 0U;
}

void boot_hook_on_crc_calculated(uint32_t crc)
{
    (void)crc;
}

#endif
