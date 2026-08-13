#ifndef BOOT_HC32F460_DRIVER_H
#define BOOT_HC32F460_DRIVER_H

#include "board.h"
#include "flash_interface.h"
#include "boot_fw_slot.h"
#include <stdbool.h>
#include <stdint.h>

/*
 * HC32F460 boot driver contract.
 * boot_port/boot_board use only boot_hc32f460_* APIs.
 * Mapped in boot_hc32f460_driver_adapter.c to RAM_* in boot_hc32f460_ram_driver.c
 */
void boot_hc32f460_spi_config(void);
bool boot_hc32f460_spi_read(uint8_t *buffer, uint32_t address, uint16_t size);
void boot_hc32f460_led_on(uint8_t port, uint16_t pin);
void boot_hc32f460_led_off(uint8_t port, uint16_t pin);
int32_t boot_hc32f460_watchdog_feed(void);
void boot_hc32f460_slider_lights(void);
void boot_hc32f460_crc_init(void);
uint32_t boot_hc32f460_crc_accumulate_data32(const uint32_t data[], uint32_t word_count);
uint32_t boot_hc32f460_crc_get(void);
void boot_hc32f460_wipe_fw_slot_and_reset(boot_fw_slot_t slot);
int32_t boot_hc32f460_flash_erase(FLASH_EraseInitTypeDef *erase_init, uint32_t *page_error);
int32_t boot_hc32f460_flash_program(uint32_t address, uint32_t data);
int32_t boot_hc32f460_port_set_bits(uint8_t port, uint16_t pin);
int32_t boot_hc32f460_port_reset_bits(uint8_t port, uint16_t pin);

/* Underlying HC32 low-level functions. */
void RAM_Spi_Config(void);
bool RAM_sFLASH_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void RAM_Board_LedOn(uint8_t port, uint16_t pin);
void RAM_Board_LedOff(uint8_t port, uint16_t pin);
int32_t RAM_WDT_FeedDog(void);
void RAM_Slider_Lights(void);
void RAM_CalcCRC32Init(void);
uint32_t RAM_CRC_AccumulateData32(const uint32_t au32Data[], uint32_t u32Len);
uint32_t RAM_CalcCRC32Get(void);
void RAM_WipeFwSlotAndReset(boot_fw_slot_t slot);
/** MODE2: WriteByteZero wipe of CAND or GOOD SPI window (no reset). */
void RAM_sFLASH_WipeFwSlot(boot_fw_slot_t slot);
void RAM_sFLASH_WriteByteZero(uint32_t WriteAddr);
int32_t HAL_FLASHEx_Erase2(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError);
int32_t HAL_FLASH_Program2(uint32_t Address, uint32_t Data);
int32_t RAM_PORT_SetBits(uint8_t enPort, uint16_t u16Pin);
int32_t RAM_PORT_ResetBits(uint8_t enPort, uint16_t u16Pin);

#include "boot_config.h"

/* Legacy aliases — prefer BOOT_APP_FLASH_* in boot_config.h */
#ifndef BOOT_HC32F460_APP_START
#define BOOT_HC32F460_APP_START      BOOT_APP_FLASH_START
#endif

#ifndef BOOT_HC32F460_APP_END
#define BOOT_HC32F460_APP_END        BOOT_APP_FLASH_END
#endif

#ifndef BOOT_HC32F460_FLASH_PAGE_SIZE
#define BOOT_HC32F460_FLASH_PAGE_SIZE BOOT_APP_FLASH_PAGE_SIZE
#endif

#endif
