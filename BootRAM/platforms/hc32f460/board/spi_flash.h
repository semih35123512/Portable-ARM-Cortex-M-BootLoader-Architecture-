/**
 * @file spi_flash.h
 * @brief SPI NOR opcodes + unit macros for HC32 reference IAP driver.
 */
#ifndef BOOT_HC32F460_SPI_FLASH_H
#define BOOT_HC32F460_SPI_FLASH_H

#include <stdint.h>
#include <stdbool.h>
#include "hc32_ll.h"
#include "hc32_ll_gpio.h"
#include "hc32_ll_spi.h"

#define SPI_FLASH_CHIP_ITEM                 2
#define sFLASH_SPI_SECTORCOUNT_DEFAULT      1024U

#define sFLASH_CMD_WriteEnable              0x06
#define sFLASH_CMD_WriteDisable             0x04
#define sFLASH_CMD_ReadStatusReg            0x05
#define sFLASH_CMD_ReadData                 0x03
#define sFLASH_CMD_ReadData4ByteMode        0x13
#define sFLASH_CMD_FastReadData             0x0B
#define sFLASH_CMD_PageProgram              0x02
#define sFLASH_CMD_PageProgram4ByteMode     0x12
#define sFLASH_CMD_SectorErase              0x20
#define sFLASH_CMD_SectorErase4ByteMode     0x21
#define sFLASH_CMD_BlockErase32             0x52
#define sFLASH_CMD_BlockErase64             0xD8
#define sFLASH_CMD_ChipErase                0xC7
#define sFLASH_CMD_JedecDeviceID            0x9F
#define sFLASH_CMD_4ByteModeEnable          0xB7
#define sFLASH_CMD_4ByteModeDisable         0xE9
#define sFLASH_CMD_ReleasePowerDown         0xAB

#define SPI_SCK_FUNC                        (GPIO_FUNC_43)
#define SPI_MOSI_FUNC                       (GPIO_FUNC_40)
#define SPI_MISO_FUNC                       (GPIO_FUNC_41)

#define SPI_UNIT                            (CM_SPI1)
#define SPI_UNIT_CLOCK                      (FCG1_PERIPH_SPI1)

extern bool sFLASH_SPI_4BYTE_ADR_MODE[SPI_FLASH_CHIP_ITEM];
extern uint32_t sFLASH_SPI_CHIPSIZE[SPI_FLASH_CHIP_ITEM];
extern uint32_t sFLASH_SPI_SECTORCOUNT_TOTAL;

bool sFLASH_EraseSector(uint32_t SectorAddr);
bool sFLASH_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
bool sFLASH_addWriteTaskToSPIFlash(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite,
                                   bool cleanWrite, bool saveEnd, bool eraseSectorEnable);
bool sFLASH_DMACheckProcessFinish(void);

/** APP SPI NOR page program via DMA (256 B typical). Returns 0 on success. */
int32_t SpiFlash_WritePageDMA(uint32_t u32Addr, uint8_t pData[], uint16_t len, bool shortCMD);

#endif
