/**
 * @file boot_target_config.h
 * @brief STM32F4 generic memory map (edit for your part / flash size).
 */
#ifndef BOOT_TARGET_CONFIG_H
#define BOOT_TARGET_CONFIG_H

#define BOOT_STUB_FLASH_START          (0x08000000UL)
#define BOOT_STUB_FLASH_SIZE           (0x00004000UL)
#define BOOT_META_FLASH_ADDR           (0x08002000UL)
#define BOOT_META_FLASH_PAGE_SIZE      (0x00002000UL)
#define BOOT_APP_FLASH_END             (0x08080000UL)
#define BOOT_APP_FLASH_PAGE_SIZE       (0x2000UL)

#define BOOT_SRAM_START                (0x20000000UL)
#define BOOT_SRAM_END                  (0x20020000UL)

#define BOOT_LINKER_IROM_START         (0x08000000UL)
#define BOOT_LINKER_IROM_SIZE          (0x00100000UL)
#define BOOT_LINKER_IRAM_START         (0x20000000UL)
#define BOOT_LINKER_IRAM_SIZE          (0x00020000UL)

#endif /* BOOT_TARGET_CONFIG_H */
