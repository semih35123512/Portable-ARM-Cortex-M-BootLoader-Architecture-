/**
 * @file boot_fw_zone_hc32f460.c
 * @brief MODE2: erase GOOD window → copy CAND→GOOD (256 B pages) → wipe CAND.
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
    uint16_t sec;
    uint32_t remain;
    uint32_t src_base;
    uint32_t dst_base;
    uint32_t off;
    uint32_t page_left;
    uint16_t chunk;
    uint8_t page[256];
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

    /* 1-based sector N => byte address (N-1) * sector_size (same as IAP). */
////    cand_span = (((uint32_t)BOOT_SPI_FW_CAND_SECTOR_LAST - (uint32_t)BOOT_SPI_FW_CAND_SECTOR_FIRST) + 1U)
////                * (uint32_t)BOOT_SPI_SECTOR_SIZE;
////    good_span = (((uint32_t)BOOT_SPI_FW_GOOD_SECTOR_LAST - (uint32_t)BOOT_SPI_FW_GOOD_SECTOR_FIRST) + 1U)
////                * (uint32_t)BOOT_SPI_SECTOR_SIZE;
////    if(((uint32_t)meta.good_payload_start + remain > cand_span) ||
////       ((uint32_t)meta.good_payload_start + remain > good_span))
////    {
////        return false;
////    }

    short_cmd = true;

    /* 1) Erase GOOD window: every sector from FIRST through LAST. */
    for(sec = (uint16_t)BOOT_SPI_FW_GOOD_SECTOR_FIRST;
        sec <= (uint16_t)BOOT_SPI_FW_GOOD_SECTOR_LAST;
        sec++)
    {
        if(!sFLASH_EraseSector(((uint32_t)sec ) * (uint32_t)BOOT_SPI_SECTOR_SIZE))
        {
            return false;
        }
    }

    /* 2) Copy CAND → GOOD in 256-byte pages (do not cross NOR page boundary). */
    src_base = (((uint32_t)BOOT_SPI_FW_CAND_SECTOR_FIRST ) * (uint32_t)BOOT_SPI_SECTOR_SIZE)
               + (uint32_t)meta.good_payload_start;
    dst_base = (((uint32_t)BOOT_SPI_FW_GOOD_SECTOR_FIRST ) * (uint32_t)BOOT_SPI_SECTOR_SIZE)
               + (uint32_t)meta.good_payload_start;

    off = 0U;
    while(remain > 0U)
    {
        page_left = 256U - ((src_base + off) & 255U);
        chunk = 256U;
        if((uint32_t)chunk > page_left)
        {
            chunk = (uint16_t)page_left;
        }
        if((uint32_t)chunk > remain)
        {
            chunk = (uint16_t)remain;
        }

        if(!RAM_sFLASH_ReadBuffer(page, src_base + off, chunk))
        {
            return false;
        }
        if(SpiFlash_WritePageDMA(dst_base + off, page, chunk, short_cmd) != 0)
        {
            return false;
        }

        off += (uint32_t)chunk;
        remain -= (uint32_t)chunk;
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
