#include "boot_mcu_select.h"
#if defined(BOOT_MCU_STM32F4_GENERIC)

#include "boot_port.h"
#include "stm32f4xx_hal.h"
#include <string.h>

/*
 * STM32F4 (F401/F407/F446) generic sample implementation.
 * - Keep boot core unchanged
 * - Customize only this file for board/storage details.
 */

#ifndef BOOT_STM32F4_APP_START
#define BOOT_STM32F4_APP_START        (0x08000000UL)
#endif

#if defined(STM32F401xC)
#define BOOT_STM32F4_DEVICE_FLASH_END_EXCL (0x08040000UL)
#elif defined(STM32F401xE)
#define BOOT_STM32F4_DEVICE_FLASH_END_EXCL (0x08080000UL)
#elif defined(STM32F407xx)
#define BOOT_STM32F4_DEVICE_FLASH_END_EXCL (0x08100000UL)
#elif defined(STM32F446xx)
#define BOOT_STM32F4_DEVICE_FLASH_END_EXCL (0x08080000UL)
#endif

#ifndef BOOT_STM32F4_APP_END
#if defined(BOOT_STM32F4_DEVICE_FLASH_END_EXCL)
#define BOOT_STM32F4_APP_END          (BOOT_STM32F4_DEVICE_FLASH_END_EXCL)
#else
#define BOOT_STM32F4_APP_END          (0x08080000UL)
#endif
#endif

#ifndef BOOT_STM32F4_TRANSFER_SIZE
#define BOOT_STM32F4_TRANSFER_SIZE    (8192UL)
#endif

#ifndef BOOT_STM32F4_SECTOR_SIZE
#define BOOT_STM32F4_SECTOR_SIZE      (4096UL)
#endif

#ifndef BOOT_STM32F4_SECTOR_HEADER_SIZE
#define BOOT_STM32F4_SECTOR_HEADER_SIZE (192UL)
#endif

#ifndef BOOT_STM32F4_NEXT_NODE_OFFSET
#define BOOT_STM32F4_NEXT_NODE_OFFSET (7UL)
#endif

#ifndef BOOT_STM32F4_FLASH_PAGE_SIZE
/* F4 sector sizes differ, but boot core increments write pointer with this value.
 * Set this macro according to your chosen erase/program stride.
 */
#define BOOT_STM32F4_FLASH_PAGE_SIZE  (8192UL)
#endif

#if !defined(BOOT_STM32F4_DEVICE_FLASH_END_EXCL)
#error "Define STM32F401xC/STM32F401xE/STM32F407xx/STM32F446xx for BOOT_MCU_STM32F4_GENERIC."
#endif

#if (BOOT_STM32F4_APP_START < 0x08000000UL)
#error "BOOT_STM32F4_APP_START must be in internal flash range."
#endif

#if (BOOT_STM32F4_APP_END > BOOT_STM32F4_DEVICE_FLASH_END_EXCL)
#error "BOOT_STM32F4_APP_END exceeds selected STM32F4 flash size."
#endif

#if (BOOT_STM32F4_APP_END <= BOOT_STM32F4_APP_START)
#error "BOOT_STM32F4_APP_END must be greater than BOOT_STM32F4_APP_START."
#endif

static const boot_port_cfg_t g_boot_port_cfg = {
    .app_start_address = BOOT_STM32F4_APP_START,
    .app_end_address = BOOT_STM32F4_APP_END,
    .flash_page_size = BOOT_STM32F4_FLASH_PAGE_SIZE,
    .transfer_size = BOOT_STM32F4_TRANSFER_SIZE,
    .sector_header_size = BOOT_STM32F4_SECTOR_HEADER_SIZE,
    .sector_size = BOOT_STM32F4_SECTOR_SIZE,
    .next_node_offset = BOOT_STM32F4_NEXT_NODE_OFFSET,
};

static uint32_t g_boot_crc32;

__weak void boot_stm32f4_extflash_init(void)
{
    /* User override: initialize external SPI flash interface */
}

__weak int boot_stm32f4_extflash_read(uint32_t address, uint8_t *buffer, uint16_t size)
{
    (void)address;
    memset(buffer, 0xFF, size);
    return 0;
}

__weak void boot_stm32f4_board_set_erase_leds(void)
{
    /* User override: board-specific erase LED pattern */
}

__weak void boot_stm32f4_board_progress_tick(void)
{
    /* User override: board-specific progress indication */
}

__weak void boot_stm32f4_watchdog_feed(void)
{
#if defined(HAL_IWDG_MODULE_ENABLED)
    extern IWDG_HandleTypeDef hiwdg;
    (void)HAL_IWDG_Refresh(&hiwdg);
#endif
}

static int boot_stm32f4_is_range_valid(uint32_t start, uint32_t end_exclusive)
{
    if((start >= end_exclusive) || (start < BOOT_STM32F4_APP_START) || (end_exclusive > BOOT_STM32F4_APP_END))
    {
        return 0;
    }
    return 1;
}

static uint32_t boot_stm32f4_address_to_sector(uint32_t address)
{
#if defined(STM32F401xC) || defined(STM32F401xE)
    if(address < 0x08004000UL) return FLASH_SECTOR_0;
    if(address < 0x08008000UL) return FLASH_SECTOR_1;
    if(address < 0x0800C000UL) return FLASH_SECTOR_2;
    if(address < 0x08010000UL) return FLASH_SECTOR_3;
    if(address < 0x08020000UL) return FLASH_SECTOR_4;
    if(address < 0x08040000UL) return FLASH_SECTOR_5;
    if(address < 0x08060000UL) return FLASH_SECTOR_6;
    return FLASH_SECTOR_7;
#elif defined(STM32F407xx) || defined(STM32F446xx)
    if(address < 0x08004000UL) return FLASH_SECTOR_0;
    if(address < 0x08008000UL) return FLASH_SECTOR_1;
    if(address < 0x0800C000UL) return FLASH_SECTOR_2;
    if(address < 0x08010000UL) return FLASH_SECTOR_3;
    if(address < 0x08020000UL) return FLASH_SECTOR_4;
    if(address < 0x08040000UL) return FLASH_SECTOR_5;
    if(address < 0x08060000UL) return FLASH_SECTOR_6;
    if(address < 0x08080000UL) return FLASH_SECTOR_7;
    if(address < 0x080A0000UL) return FLASH_SECTOR_8;
    if(address < 0x080C0000UL) return FLASH_SECTOR_9;
    if(address < 0x080E0000UL) return FLASH_SECTOR_10;
    return FLASH_SECTOR_11;
#else
#error "Define STM32F401x*/STM32F407xx/STM32F446xx for BOOT_MCU_STM32F4_GENERIC."
#endif
}

void boot_port_disable_irq(void)
{
    __disable_irq();
}

void boot_port_spi_config(void)
{
    boot_stm32f4_extflash_init();
}

void boot_port_spi_read(uint8_t *buffer, uint32_t address, uint16_t size)
{
    if((buffer == NULL) || (size == 0U))
    {
        return;
    }
    if(boot_stm32f4_extflash_read(address, buffer, size) != 0)
    {
        memset(buffer, 0xFF, size);
    }
}

void boot_port_flash_erase_app_area(void)
{
    FLASH_EraseInitTypeDef eraseInit = {0};
    uint32_t pageError = 0;
    uint32_t startAddress = g_boot_port_cfg.app_start_address;
    uint32_t endAddress = g_boot_port_cfg.app_end_address;
    uint32_t startSector;
    uint32_t endSector;

    if(!boot_stm32f4_is_range_valid(startAddress, endAddress))
    {
        return;
    }
    startSector = boot_stm32f4_address_to_sector(startAddress);
    endSector = boot_stm32f4_address_to_sector(endAddress - 1UL);

    boot_stm32f4_board_set_erase_leds();
    HAL_FLASH_Unlock();
    eraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
    eraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    eraseInit.Sector = startSector;
    eraseInit.NbSectors = (endSector - startSector) + 1UL;
    (void)HAL_FLASHEx_Erase(&eraseInit, &pageError);
    HAL_FLASH_Lock();
}

void boot_port_flash_program_page(uint32_t flash_address, const uint32_t *data)
{
    uint32_t words = (uint32_t)(g_boot_port_cfg.transfer_size / 4UL);
    uint32_t rangeEnd = flash_address + g_boot_port_cfg.transfer_size;

    if((data == NULL) || ((flash_address & 0x3UL) != 0UL) || (!boot_stm32f4_is_range_valid(flash_address, rangeEnd)))
    {
        return;
    }

    HAL_FLASH_Unlock();
    for(uint32_t i = 0; i < words; i++)
    {
        (void)HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flash_address + (i * 4UL), data[i]);
    }
    HAL_FLASH_Lock();
}

void boot_port_set_erase_leds(void)
{
    boot_stm32f4_board_set_erase_leds();
}

void boot_port_feed_watchdog(void)
{
    boot_stm32f4_watchdog_feed();
}

void boot_port_slider_lights(void)
{
    boot_stm32f4_board_progress_tick();
}

void boot_port_crc_init(void)
{
    g_boot_crc32 = 0xFFFFFFFFUL;
}

void boot_port_crc_accumulate32(const uint32_t *data, uint32_t word_count)
{
    if((data == NULL) || (word_count == 0U))
    {
        return;
    }

    for(uint32_t w = 0U; w < word_count; w++)
    {
        uint32_t value = data[w];
        for(uint32_t b = 0U; b < 4U; b++)
        {
            uint8_t byte = (uint8_t)((value >> (8U * b)) & 0xFFU);
            g_boot_crc32 ^= byte;
            for(uint32_t i = 0U; i < 8U; i++)
            {
                if((g_boot_crc32 & 1U) != 0U)
                {
                    g_boot_crc32 = (g_boot_crc32 >> 1U) ^ 0xEDB88320UL;
                }
                else
                {
                    g_boot_crc32 >>= 1U;
                }
            }
        }
    }
}

uint32_t boot_port_crc_get(void)
{
    return ~g_boot_crc32;
}

void boot_port_fail_and_reset(boot_fw_slot_t slot)
{
    (void)slot;
    NVIC_SystemReset();
}

const boot_port_cfg_t *boot_port_get_cfg(void)
{
    return &g_boot_port_cfg;
}

#endif
