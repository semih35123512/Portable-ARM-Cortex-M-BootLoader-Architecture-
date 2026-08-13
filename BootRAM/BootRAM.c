/**
 * @file BootRAM.c
 * @brief Portable facade - IAP entry only.
 *
 * MCU-specific RAM drivers (SPI/CRC/GPIO/EFM/WDT) live under:
 *   platforms/<mcu>/
 * For HC32F460: platforms/hc32f460/boot_hc32f460_ram_driver.c
 *
 * After FOTA image is staged on SPI, call RAM_ApplySpiFirmware with size,
 * payload start offset, image CRC and slot - meta cand/good fields are
 * latched here before erase/program.
 */
#include "boot_mcu_select.h"
#include "boot_core.h"
#include "boot_ram_attr.h"
#include "boot_fw_slot.h"
#include "boot_meta.h"
#include "boot_config.h"
#include "boot_hooks.h"

#if defined(BOOT_MCU_HC32F460)
#include "platforms/hc32f460/BootRAM.h"
#endif

BOOT_RAM_FUNC void RAM_ApplySpiFirmware(uint32_t binaryFileSize,
                                      uint16_t fileBinStartPoint,
                                      uint32_t imageCrc,
                                      boot_fw_slot_t slot)
{
    uint16_t start_node = 0U;

    boot_hook_set_fw_slot(slot);

#if (BOOT_SPI_FW_MODE == BOOT_SPI_FW_MODE_FIXED)
    if(slot == BOOT_FW_SLOT_GOOD)
    {
        start_node = (uint16_t)BOOT_SPI_FW_GOOD_SECTOR_FIRST;
    }
    else
    {
        start_node = (uint16_t)BOOT_SPI_FW_CAND_SECTOR_FIRST;
    }
#else
    if(!boot_hook_get_firmware_start_node(&start_node))
    {
        start_node = 0U;
    }
#endif

    /* Latch cand_size / cand_payload_start / cand_crc (or good fields) before IAP. */
    (void)boot_meta_begin_update(slot,
                                 binaryFileSize,
                                 fileBinStartPoint,
                                 start_node,
                                 imageCrc);

    boot_core_firmware_update(binaryFileSize, fileBinStartPoint, slot);
}
