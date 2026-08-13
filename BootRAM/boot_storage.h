#ifndef BOOT_STORAGE_H
#define BOOT_STORAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "boot_ram_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SPI firmware accessors for IAP (see BOOT_SPI_FW_MODE in boot_config.h).
 *
 * MODE1 (ZONE): node = zone sector id; payload after 192 B header; next @ +7.
 * MODE2 (FIXED): node = absolute 1-based sector in GOOD/CANDIDATE window;
 *                raw payload (no header); next = sector + 1 within window.
 */
BOOT_RAM_FUNC bool boot_storage_get_first_firmware_node(uint16_t *node);
BOOT_RAM_FUNC bool boot_storage_read_next_node(uint16_t current_node, uint16_t *next_node);
BOOT_RAM_FUNC bool boot_storage_read_node_payload(uint16_t node, uint32_t payload_offset, uint8_t *buffer, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif /* BOOT_STORAGE_H */
