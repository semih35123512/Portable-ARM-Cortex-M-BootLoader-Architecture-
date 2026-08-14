/**
 * @file boot_port_user_impl.c
 * @brief Copy this folder and implement for your ARM Cortex-M MCU.
 * Define BOOT_MCU_USER in your project.
 */
#include "boot_mcu_select.h"
#if defined(BOOT_MCU_USER)

#include "boot_port.h"
#include "boot_config.h"
#include "boot_ram_attr.h"

static const boot_port_cfg_t g_boot_port_cfg = {
    .app_start_address = BOOT_APP_FLASH_START,
    .app_end_address = BOOT_APP_FLASH_END,
    .flash_page_size = BOOT_APP_FLASH_PAGE_SIZE,
    .transfer_size = BOOT_TRANSFER_SIZE,
    .sector_header_size = BOOT_SPI_SECTOR_HEADER_SIZE,
    .sector_size = BOOT_SPI_SECTOR_SIZE,
    .next_node_offset = BOOT_SPI_NEXT_NODE_OFFSET,
};

BOOT_RAM_FUNC void boot_port_disable_irq(void)
{
    /* TODO: __disable_irq(); */
}

BOOT_RAM_FUNC void boot_port_iap_gpio_init(void)
{
    /* TODO: IAP LED / external WDT GPIO as outputs (needed on stub rollback). */
}

BOOT_RAM_FUNC void boot_port_spi_config(void)
{
    /* TODO: SPI + CS init for external flash (must be callable from RAM). */
}

BOOT_RAM_FUNC void boot_port_spi_read(uint8_t *buffer, uint32_t address, uint16_t size)
{
    (void)buffer;
    (void)address;
    (void)size;
    /* TODO: blocking read from external SPI NOR. */
}

BOOT_RAM_FUNC void boot_port_flash_erase_app_area(void)
{
    /* TODO: erase internal flash [BOOT_APP_FLASH_START, BOOT_APP_FLASH_END). */
}

BOOT_RAM_FUNC void boot_port_flash_program_page(uint32_t flash_address, const uint32_t *data)
{
    (void)flash_address;
    (void)data;
    /* TODO: program one transfer chunk (BOOT_TRANSFER_SIZE) at flash_address. */
}

BOOT_RAM_FUNC void boot_port_set_erase_leds(void) {}
BOOT_RAM_FUNC void boot_port_feed_watchdog(void) { /* TODO */ }
BOOT_RAM_FUNC void boot_port_slider_lights(void) {}
BOOT_RAM_FUNC void boot_port_crc_init(void) { /* TODO: hardware or software CRC32 */ }
BOOT_RAM_FUNC void boot_port_crc_accumulate32(const uint32_t *data, uint32_t word_count)
{
    (void)data;
    (void)word_count;
}
BOOT_RAM_FUNC uint32_t boot_port_crc_get(void) { return 0U; }
BOOT_RAM_FUNC void boot_port_finish_and_reset(boot_fw_slot_t slot, uint8_t success)
{
    (void)slot;
    (void)success;
    /* TODO: NVIC_SystemReset(); */
}

const boot_port_cfg_t *boot_port_get_cfg(void)
{
    return &g_boot_port_cfg;
}

#endif
