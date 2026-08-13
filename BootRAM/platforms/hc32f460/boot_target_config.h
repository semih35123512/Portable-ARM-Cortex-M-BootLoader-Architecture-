/**
 * @file boot_target_config.h
 * @brief HC32F460 memory map + linker regions (512 KB on-chip flash).
 *
 *   [0x00000000 .. 0x00001FFF]  Boot stub + IAP RAM load image
 *   [0x00002000 .. 0x00003FFF]  boot_meta (8 KB page)
 *   [0x00004000 .. 0x00077FFF]  Application
 *   [0x00078000 .. ]            EEPROM emulator (unchanged)
 */
#ifndef BOOT_TARGET_CONFIG_H
#define BOOT_TARGET_CONFIG_H

#define BOOT_STUB_FLASH_START          (0x00000000UL)
#define BOOT_STUB_FLASH_SIZE           (0x00004000UL)
#define BOOT_META_FLASH_ADDR           (0x00002000UL)
#define BOOT_META_FLASH_PAGE_SIZE      (0x00002000UL)
#define BOOT_APP_FLASH_END             (0x00078000UL)
#define BOOT_APP_FLASH_PAGE_SIZE       (0x2000UL)

#define BOOT_SRAM_START                (0x1FFF8000UL)
#define BOOT_SRAM_END                  (0x20028000UL)

/* Keil scatter / device dialog */
#define BOOT_LINKER_IROM_START         (0x00000000UL)
#define BOOT_LINKER_IROM_SIZE          (0x00080000UL)
#define BOOT_LINKER_IRAM_START         (0x1FFF8000UL)
#define BOOT_LINKER_IRAM_SIZE          (0x0002E000UL)

#endif /* BOOT_TARGET_CONFIG_H */
