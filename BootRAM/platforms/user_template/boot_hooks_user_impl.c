#include "boot_mcu_select.h"
#if defined(BOOT_MCU_USER)

#include "boot_hooks.h"
#include "boot_config.h"

/* Set BOOT_FW_START_NODE in boot_config.h OR fill this hook from your FS/metadata. */
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
    return 0U; /* TODO: CRC from download header / host */
}

void boot_hook_on_crc_calculated(uint32_t crc)
{
    (void)crc;
}

#endif
