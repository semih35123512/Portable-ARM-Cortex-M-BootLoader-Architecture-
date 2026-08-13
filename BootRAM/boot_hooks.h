/**
 * @file boot_hooks.h
 * @brief Thin product/MCU hooks — implement in platforms/<mcu>/ or user_template.
 */
#ifndef BOOT_HOOKS_H
#define BOOT_HOOKS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "boot_fw_slot.h"

#ifdef __cplusplus
extern "C" {
#endif

void boot_hook_set_fw_slot(boot_fw_slot_t slot);
boot_fw_slot_t boot_hook_get_fw_slot(void);

/**
 * @brief Return first SPI zone node for the active firmware slot.
 */
bool boot_hook_get_firmware_start_node(uint16_t *node);

/**
 * @brief Expected CRC32 of the active slot image.
 */
uint32_t boot_hook_get_expected_image_crc(void);

/**
 * @brief Publish calculated CRC (optional; product may mirror to a global).
 */
void boot_hook_on_crc_calculated(uint32_t crc);

#ifdef __cplusplus
}
#endif

#endif /* BOOT_HOOKS_H */
