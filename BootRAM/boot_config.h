/**
 * @file boot_config.h
 * @brief Portable IAP configuration (SPI layout, transfer sizes, validation).
 *
 * MCU flash / SRAM addresses live in platforms/<mcu>/boot_target_config.h
 * (included via boot_target_config_select.h). Edit memory map there, not here.
 */
#ifndef BOOT_CONFIG_H
#define BOOT_CONFIG_H

#include <stdint.h>
#include "boot_target_config_select.h"

#define BOOT_STUB_FLASH_END            (BOOT_STUB_FLASH_START + BOOT_STUB_FLASH_SIZE)

#ifndef BOOT_APP_FLASH_START
#define BOOT_APP_FLASH_START           (BOOT_STUB_FLASH_END)
#endif

#ifndef VECT_TAB_OFFSET
#define VECT_TAB_OFFSET                (BOOT_APP_FLASH_START)
#endif

#ifndef BOOT_SPI_SECTOR_SIZE
#define BOOT_SPI_SECTOR_SIZE           (4096U)
#endif

#ifndef BOOT_SPI_SECTOR_HEADER_SIZE
#define BOOT_SPI_SECTOR_HEADER_SIZE    (192U)
#endif

#ifndef BOOT_SPI_NEXT_NODE_OFFSET
#define BOOT_SPI_NEXT_NODE_OFFSET      (7U)
#endif

/*------------------------------------------------------------------------------
 * SPI firmware image layout for bootloader IAP read
 *
 * MODE1 (ZONE): linked sectors, 192 B header, next@+7
 * MODE2 (FIXED): contiguous raw binary in fixed SPI sector ranges
 *----------------------------------------------------------------------------*/
#define BOOT_SPI_FW_MODE_ZONE          (1U)
#define BOOT_SPI_FW_MODE_FIXED         (2U)

#ifndef BOOT_SPI_FW_MODE
#define BOOT_SPI_FW_MODE               BOOT_SPI_FW_MODE_FIXED
#endif

#ifndef BOOT_SPI_FW_CAND_SECTOR_FIRST
#define BOOT_SPI_FW_CAND_SECTOR_FIRST  (769U)
#endif
#ifndef BOOT_SPI_FW_CAND_SECTOR_LAST
#define BOOT_SPI_FW_CAND_SECTOR_LAST   (896U)
#endif
#ifndef BOOT_SPI_FW_GOOD_SECTOR_FIRST
#define BOOT_SPI_FW_GOOD_SECTOR_FIRST  (897U)
#endif
#ifndef BOOT_SPI_FW_GOOD_SECTOR_LAST
#define BOOT_SPI_FW_GOOD_SECTOR_LAST   (1024U)
#endif

#if (BOOT_SPI_FW_MODE != BOOT_SPI_FW_MODE_ZONE) && (BOOT_SPI_FW_MODE != BOOT_SPI_FW_MODE_FIXED)
#error "BOOT_SPI_FW_MODE must be BOOT_SPI_FW_MODE_ZONE or BOOT_SPI_FW_MODE_FIXED"
#endif

#if (BOOT_SPI_FW_MODE == BOOT_SPI_FW_MODE_FIXED)
#if (BOOT_SPI_FW_CAND_SECTOR_FIRST == 0U) || (BOOT_SPI_FW_GOOD_SECTOR_FIRST == 0U)
#error "MODE2 sector numbers are 1-based (min 1)"
#endif
#if (BOOT_SPI_FW_CAND_SECTOR_FIRST > BOOT_SPI_FW_CAND_SECTOR_LAST)
#error "Invalid CANDIDATE sector range"
#endif
#if (BOOT_SPI_FW_GOOD_SECTOR_FIRST > BOOT_SPI_FW_GOOD_SECTOR_LAST)
#error "Invalid GOOD sector range"
#endif
#endif

#ifndef BOOT_TRANSFER_SIZE
#define BOOT_TRANSFER_SIZE             (8192U)
#endif

#ifndef BOOT_STREAM_BUFFER_SIZE
#define BOOT_STREAM_BUFFER_SIZE        (13U * 1024U)
#endif

#if (BOOT_TRANSFER_SIZE > BOOT_STREAM_BUFFER_SIZE)
#error "BOOT_TRANSFER_SIZE must be <= BOOT_STREAM_BUFFER_SIZE"
#endif

#if ((BOOT_TRANSFER_SIZE % 4U) != 0U)
#error "BOOT_TRANSFER_SIZE must be a multiple of 4"
#endif

#if (BOOT_APP_FLASH_START >= BOOT_APP_FLASH_END)
#error "Invalid APP flash map"
#endif

#if ((BOOT_STUB_FLASH_SIZE % BOOT_APP_FLASH_PAGE_SIZE) != 0U)
#error "BOOT_STUB_FLASH_SIZE must be a multiple of flash page size"
#endif

#if ((BOOT_META_FLASH_ADDR < BOOT_STUB_FLASH_END) && \
     (BOOT_META_FLASH_ADDR >= BOOT_APP_FLASH_START))
#error "boot_meta must not overlap APP"
#endif

#if ((BOOT_META_FLASH_ADDR + BOOT_META_FLASH_PAGE_SIZE) > BOOT_APP_FLASH_START)
#error "boot_meta page must end at or before APP start"
#endif

#ifndef BOOT_FW_START_NODE
#define BOOT_FW_START_NODE             (0xFFFFU)
#endif

#endif /* BOOT_CONFIG_H */
