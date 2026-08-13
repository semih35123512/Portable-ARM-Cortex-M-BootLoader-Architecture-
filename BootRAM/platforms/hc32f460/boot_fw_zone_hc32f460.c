/**
 * @file boot_fw_zone_hc32f460.c
 * @brief MODE2: wipe GOOD → copy CAND→GOOD (256 B DMA pages) → wipe CAND.
 *
 * MODE1 (product zone FS) is not part of the public reference; keep that
 * integration in a private product tree.
 */
#include "boot_mcu_select.h"
#if defined(BOOT_MCU_HC32F460)

#include "boot_fw_zone.h"
#include "boot_meta.h"
#include "boot_config.h"
#include "boot_fw_slot.h"
#include "spi_flash.h"
#include "boot_hc32f460_driver.h"

#if (BOOT_SPI_FW_MODE == BOOT_SPI_FW_MODE_ZONE)

bool boot_fw_zone_commit_candidate_as_good(void)
{
    /* Product zone-FS promote lives outside this public reference. */
    return false;
}

#else /* BOOT_SPI_FW_MODE_FIXED */

bool boot_fw_zone_commit_candidate_as_good(void)
{
    boot_meta_t meta;
    uint32_t remain;
    uint16_t src_sec;
    uint16_t dst_sec;
    uint16_t last_src;
    uint16_t last_dst;
    uint8_t page[256];
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t off;
    uint16_t chunk;
    bool short_cmd;

    if(!boot_meta_read(&meta) || !boot_meta_is_valid(&meta))
    {
        return false;
    }

    remain = meta.good_size;
    if(remain == 0U)
    {
        return false;
    }

    src_sec = (uint16_t)BOOT_SPI_FW_CAND_SECTOR_FIRST;
    dst_sec = (uint16_t)BOOT_SPI_FW_GOOD_SECTOR_FIRST;
    last_src = (uint16_t)BOOT_SPI_FW_CAND_SECTOR_LAST;
    last_dst = (uint16_t)BOOT_SPI_FW_GOOD_SECTOR_LAST;

    /* 3-byte cmd when chip is not in 4-byte address mode. */
    short_cmd = (sFLASH_SPI_4BYTE_ADR_MODE[0] == false);

    /* 1) Wipe old GOOD window (WriteByteZero over GOOD sector range). */
    RAM_sFLASH_WipeFwSlot(BOOT_FW_SLOT_GOOD);

    /* 2) Copy CAND → GOOD in 256-byte pages via SpiFlash_WritePageDMA. */
    while(remain > 0U)
    {
        if((src_sec > last_src) || (dst_sec > last_dst))
        {
            return false;
        }

        /* 1-based sector N => address (N-1) * sector_size */
        src_addr = ((uint32_t)(src_sec - 1U) * (uint32_t)BOOT_SPI_SECTOR_SIZE);
        dst_addr = ((uint32_t)(dst_sec - 1U) * (uint32_t)BOOT_SPI_SECTOR_SIZE);

        if(!sFLASH_EraseSector(dst_addr))
        {
            return false;
        }

        off = 0U;
        while(off < (uint32_t)BOOT_SPI_SECTOR_SIZE)
        {
            chunk = 256U;
            if(chunk > (uint16_t)((uint32_t)BOOT_SPI_SECTOR_SIZE - off))
            {
                chunk = (uint16_t)((uint32_t)BOOT_SPI_SECTOR_SIZE - off);
            }
            if((uint32_t)chunk > remain)
            {
                chunk = (uint16_t)remain;
            }
            if(chunk == 0U)
            {
                break;
            }

            if(!sFLASH_ReadBuffer(page, src_addr + off, chunk))
            {
                return false;
            }

            if(SpiFlash_WritePageDMA(dst_addr + off, page, chunk, short_cmd) != 0)
            {
                return false;
            }
            while(!sFLASH_DMACheckProcessFinish())
            {
            }

            off += (uint32_t)chunk;
            remain -= (uint32_t)chunk;
        }

        src_sec++;
        dst_sec++;
    }

    /* 3) Wipe CAND window after successful promote. */
    RAM_sFLASH_WipeFwSlot(BOOT_FW_SLOT_CANDIDATE);

    return true;
}

#endif /* BOOT_SPI_FW_MODE */

bool boot_fw_on_app_healthy(void)
{
    bool ok = boot_meta_confirm_boot_ok();
    if(boot_meta_promote_candidate_descriptors())
    {
        (void)boot_fw_zone_commit_candidate_as_good();
    }
    return ok;
}

#endif /* BOOT_MCU_HC32F460 */
