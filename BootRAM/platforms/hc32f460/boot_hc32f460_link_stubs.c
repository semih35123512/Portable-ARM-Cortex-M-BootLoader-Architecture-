/**
 * @file boot_hc32f460_link_stubs.c
 * @brief Public link stubs for standalone HC32 reference build (no product APP).
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#include "spi_flash.h"
#include "hc32_ll_spi.h"
#include "hc32_ll_def.h"

uint32_t SystemCoreClock = 200000000UL;
uint32_t HRC_VALUE = 16000000UL;

bool sFLASH_SPI_4BYTE_ADR_MODE[SPI_FLASH_CHIP_ITEM];
uint32_t sFLASH_SPI_CHIPSIZE[SPI_FLASH_CHIP_ITEM] = { (uint32_t)(1024UL * 4096UL), 0UL };
uint32_t sFLASH_SPI_SECTORCOUNT_TOTAL = 1024UL;

bool sFLASH_EraseSector(uint32_t SectorAddr)
{
    (void)SectorAddr;
    return false;
}

bool sFLASH_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    (void)pBuffer;
    (void)ReadAddr;
    (void)NumByteToRead;
    return false;
}

bool sFLASH_addWriteTaskToSPIFlash(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite,
                                   bool cleanWrite, bool saveEnd, bool eraseSectorEnable)
{
    (void)pBuffer;
    (void)WriteAddr;
    (void)NumByteToWrite;
    (void)cleanWrite;
    (void)saveEnd;
    (void)eraseSectorEnable;
    return false;
}

bool sFLASH_DMACheckProcessFinish(void)
{
    return true;
}

int32_t SpiFlash_WritePageDMA(uint32_t u32Addr, uint8_t pData[], uint16_t len, bool shortCMD)
{
    (void)u32Addr;
    (void)pData;
    (void)len;
    (void)shortCMD;
    return -1;
}

void SPI_DeInit(CM_SPI_TypeDef *SPIx)
{
    (void)SPIx;
}

int32_t SPI_Init(CM_SPI_TypeDef *SPIx, const stc_spi_init_t *pstcSpiInit)
{
    (void)SPIx;
    (void)pstcSpiInit;
    return LL_OK;
}

void SPI_ReadBufConfig(CM_SPI_TypeDef *SPIx, uint32_t u32ReadBuf)
{
    (void)SPIx;
    (void)u32ReadBuf;
}

int32_t SPI_DelayStructInit(stc_spi_delay_t *pstcDelayConfig)
{
    if(pstcDelayConfig != NULL)
    {
        (void)memset(pstcDelayConfig, 0, sizeof(*pstcDelayConfig));
    }
    return LL_OK;
}

int32_t SPI_DelayTimeConfig(CM_SPI_TypeDef *SPIx, const stc_spi_delay_t *pstcDelayConfig)
{
    (void)SPIx;
    (void)pstcDelayConfig;
    return LL_OK;
}
