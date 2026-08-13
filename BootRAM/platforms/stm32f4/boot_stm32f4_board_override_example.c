#include "boot_mcu_select.h"

#if defined(BOOT_MCU_STM32F4_GENERIC)

#include "stm32f4xx_hal.h"
#include <stdint.h>

/*
 * Example board override for STM32F4 generic boot port.
 *
 * How to use:
 * 1) Define BOOT_MCU_STM32F4_GENERIC in compiler symbols.
 * 2) Ensure STM32 device symbol is set (STM32F401x*, STM32F407xx, STM32F446xx).
 * 3) Replace SPI/CS handles and GPIO pins below with your board values.
 */

extern SPI_HandleTypeDef hspi1;
extern IWDG_HandleTypeDef hiwdg;

#ifndef BOOT_STM32F4_BOOT_SPI_HANDLE
#define BOOT_STM32F4_BOOT_SPI_HANDLE hspi1
#endif

#ifndef BOOT_STM32F4_CS_GPIO_Port
#define BOOT_STM32F4_CS_GPIO_Port GPIOA
#endif

#ifndef BOOT_STM32F4_CS_Pin
#define BOOT_STM32F4_CS_Pin GPIO_PIN_4
#endif

/* Basic SPI commands for external flash read. */
#define BOOT_FLASH_CMD_READ_DATA        (0x03U)

static void boot_stm32f4_cs_low(void)
{
    HAL_GPIO_WritePin(BOOT_STM32F4_CS_GPIO_Port, BOOT_STM32F4_CS_Pin, GPIO_PIN_RESET);
}

static void boot_stm32f4_cs_high(void)
{
    HAL_GPIO_WritePin(BOOT_STM32F4_CS_GPIO_Port, BOOT_STM32F4_CS_Pin, GPIO_PIN_SET);
}

void boot_stm32f4_extflash_init(void)
{
    /* Optional: add SPI re-init or flash wake-up sequence here. */
    boot_stm32f4_cs_high();
}

int boot_stm32f4_extflash_read(uint32_t address, uint8_t *buffer, uint16_t size)
{
    uint8_t header[4];

    if((buffer == NULL) || (size == 0U))
    {
        return -1;
    }

    header[0] = BOOT_FLASH_CMD_READ_DATA;
    header[1] = (uint8_t)((address >> 16U) & 0xFFU);
    header[2] = (uint8_t)((address >> 8U) & 0xFFU);
    header[3] = (uint8_t)(address & 0xFFU);

    boot_stm32f4_cs_low();
    if(HAL_SPI_Transmit(&BOOT_STM32F4_BOOT_SPI_HANDLE, header, (uint16_t)sizeof(header), 100U) != HAL_OK)
    {
        boot_stm32f4_cs_high();
        return -2;
    }
    if(HAL_SPI_Receive(&BOOT_STM32F4_BOOT_SPI_HANDLE, buffer, size, 200U) != HAL_OK)
    {
        boot_stm32f4_cs_high();
        return -3;
    }
    boot_stm32f4_cs_high();
    return 0;
}

void boot_stm32f4_watchdog_feed(void)
{
    (void)HAL_IWDG_Refresh(&hiwdg);
}

void boot_stm32f4_board_set_erase_leds(void)
{
    /* Optional: board-specific LED pattern during erase. */
}

void boot_stm32f4_board_progress_tick(void)
{
    /* Optional: board-specific progress tick during programming. */
}

#endif

