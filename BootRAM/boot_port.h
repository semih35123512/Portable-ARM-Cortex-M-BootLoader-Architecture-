#ifndef BOOT_PORT_H
#define BOOT_PORT_H

#include <stdint.h>
#include "boot_config.h"
#include "boot_ram_attr.h"
#include "boot_fw_slot.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MCU-specific geometry returned to core (normally filled from boot_config.h).
 */
typedef struct
{
    uint32_t app_start_address;
    uint32_t app_end_address;
    uint32_t flash_page_size;
    uint32_t transfer_size;
    uint32_t sector_header_size;
    uint32_t sector_size;
    uint32_t next_node_offset;
} boot_port_cfg_t;

/* ---- Implement these in platforms/<mcu>/boot_port_*_impl.c ---- */

BOOT_RAM_FUNC void boot_port_disable_irq(void);
BOOT_RAM_FUNC void boot_port_spi_config(void);
BOOT_RAM_FUNC void boot_port_spi_read(uint8_t *buffer, uint32_t address, uint16_t size);
BOOT_RAM_FUNC void boot_port_flash_erase_app_area(void);
BOOT_RAM_FUNC void boot_port_flash_program_page(uint32_t flash_address, const uint32_t *data);
BOOT_RAM_FUNC void boot_port_set_erase_leds(void);
BOOT_RAM_FUNC void boot_port_feed_watchdog(void);
BOOT_RAM_FUNC void boot_port_slider_lights(void);
BOOT_RAM_FUNC void boot_port_crc_init(void);
BOOT_RAM_FUNC void boot_port_crc_accumulate32(const uint32_t *data, uint32_t word_count);
BOOT_RAM_FUNC uint32_t boot_port_crc_get(void);
/** Wipe selected SPI FW window (CAND/GOOD) then reset. */
BOOT_RAM_FUNC void boot_port_fail_and_reset(boot_fw_slot_t slot);

const boot_port_cfg_t *boot_port_get_cfg(void);

#ifdef __cplusplus
}
#endif

#endif /* BOOT_PORT_H */
