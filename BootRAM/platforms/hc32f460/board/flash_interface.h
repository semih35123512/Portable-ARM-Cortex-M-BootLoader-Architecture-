/**
 * @file flash_interface.h
 * @brief Minimal flash erase/program types for HC32 reference port.
 */
#ifndef BOOT_HC32F460_FLASH_INTERFACE_H
#define BOOT_HC32F460_FLASH_INTERFACE_H

#include <stdint.h>

#define FLASH_TYPEERASE_PAGES     ((uint32_t)0x00)
#define FLASH_TYPEERASE_MASSERASE ((uint32_t)0x01)
#define FLASH_BANK_1              ((uint32_t)0x01)
#define FLASH_PAGE_SIZE           ((uint32_t)0x2000)

#ifndef FLASH_CACHE_DISABLED
#define FLASH_CACHE_DISABLED      0x08
#endif

typedef struct
{
    uint32_t TypeErase;
    uint32_t Banks;
    uint32_t Page;
    uint32_t NbPages;
} FLASH_EraseInitTypeDef;

#endif
