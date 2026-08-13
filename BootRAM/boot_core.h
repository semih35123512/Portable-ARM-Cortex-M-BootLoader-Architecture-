/**
 * @file boot_core.h
 * @brief MCU-independent IAP core.
 */
#ifndef BOOT_CORE_H
#define BOOT_CORE_H

#include <stdint.h>
#include "boot_ram_attr.h"
#include "boot_fw_slot.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Run staged firmware update from external SPI zone flash into internal flash.
 * @param binary_file_size      Image byte size
 * @param file_bin_start_point  Payload offset inside the first sector (after header/meta)
 * @param slot                  CANDIDATE (OTA) or GOOD (stub recovery)
 *
 * Must execute from RAM (BOOT_RAM_FUNC). Does not return on success (reset via port).
 */
BOOT_RAM_FUNC void boot_core_firmware_update(uint32_t binary_file_size,
                                             uint16_t file_bin_start_point,
                                             boot_fw_slot_t slot);

#ifdef __cplusplus
}
#endif

#endif /* BOOT_CORE_H */
