#include "boot_mcu_select.h"
#if defined(BOOT_MCU_STM32_TEMPLATE)

#include "boot_port.h"
#include "boot_config.h"
#include "boot_ram_attr.h"

/*
 * Minimal STM32 / generic ARM template.
 * Edit boot_config.h first, then replace stubs below with your HAL/LL drivers.
 */

static const boot_port_cfg_t g_boot_port_cfg = {
    .app_start_address = BOOT_APP_FLASH_START,
    .app_end_address = BOOT_APP_FLASH_END,
    .flash_page_size = BOOT_APP_FLASH_PAGE_SIZE,
    .transfer_size = BOOT_TRANSFER_SIZE,
    .sector_header_size = BOOT_SPI_SECTOR_HEADER_SIZE,
    .sector_size = BOOT_SPI_SECTOR_SIZE,
    .next_node_offset = BOOT_SPI_NEXT_NODE_OFFSET,
};

BOOT_RAM_FUNC void boot_port_disable_irq(void) { /* __disable_irq(); */ }
BOOT_RAM_FUNC void boot_port_spi_config(void) { /* init SPI + CS GPIO */ }
BOOT_RAM_FUNC void boot_port_spi_read(uint8_t *buffer, uint32_t address, uint16_t size)
{
    (void)buffer;
    (void)address;
    (void)size;
    /* blocking SPI flash read into buffer */
}
BOOT_RAM_FUNC void boot_port_flash_erase_app_area(void)
{
    /* erase [BOOT_APP_FLASH_START, BOOT_APP_FLASH_END) */
}
BOOT_RAM_FUNC void boot_port_flash_program_page(uint32_t flash_address, const uint32_t *data)
{
    (void)flash_address;
    (void)data;
    /* program BOOT_TRANSFER_SIZE bytes (or page) at flash_address */
}
BOOT_RAM_FUNC void boot_port_set_erase_leds(void) {}
BOOT_RAM_FUNC void boot_port_feed_watchdog(void) {}
BOOT_RAM_FUNC void boot_port_slider_lights(void) {}
BOOT_RAM_FUNC void boot_port_crc_init(void) {}
BOOT_RAM_FUNC void boot_port_crc_accumulate32(const uint32_t *data, uint32_t word_count)
{
    (void)data;
    (void)word_count;
}
BOOT_RAM_FUNC uint32_t boot_port_crc_get(void) { return 0U; }
BOOT_RAM_FUNC void boot_port_fail_and_reset(boot_fw_slot_t slot) { (void)slot; /* NVIC_SystemReset(); */ }

const boot_port_cfg_t *boot_port_get_cfg(void)
{
    return &g_boot_port_cfg;
}

#endif
