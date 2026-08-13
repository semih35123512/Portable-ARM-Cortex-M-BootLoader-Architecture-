/**
 * @file boot_target_config.h
 * @brief Minimal Cortex-M template — override all values for your MCU.
 */
#ifndef BOOT_TARGET_CONFIG_H
#define BOOT_TARGET_CONFIG_H

#ifndef BOOT_STUB_FLASH_START
#define BOOT_STUB_FLASH_START          (0x08000000UL)
#endif
#ifndef BOOT_STUB_FLASH_SIZE
#define BOOT_STUB_FLASH_SIZE           (0x00004000UL)
#endif
#ifndef BOOT_META_FLASH_ADDR
#define BOOT_META_FLASH_ADDR           (BOOT_STUB_FLASH_START + 0x2000UL)
#endif
#ifndef BOOT_META_FLASH_PAGE_SIZE
#define BOOT_META_FLASH_PAGE_SIZE      (0x00002000UL)
#endif
#ifndef BOOT_APP_FLASH_END
#define BOOT_APP_FLASH_END             (BOOT_STUB_FLASH_START + 0x00080000UL)
#endif
#ifndef BOOT_APP_FLASH_PAGE_SIZE
#define BOOT_APP_FLASH_PAGE_SIZE       (0x2000UL)
#endif

#ifndef BOOT_SRAM_START
#define BOOT_SRAM_START                (0x20000000UL)
#endif
#ifndef BOOT_SRAM_END
#define BOOT_SRAM_END                  (0x20020000UL)
#endif

#ifndef BOOT_LINKER_IROM_START
#define BOOT_LINKER_IROM_START         (BOOT_STUB_FLASH_START)
#endif
#ifndef BOOT_LINKER_IROM_SIZE
#define BOOT_LINKER_IROM_SIZE          (0x00080000UL)
#endif
#ifndef BOOT_LINKER_IRAM_START
#define BOOT_LINKER_IRAM_START         (BOOT_SRAM_START)
#endif
#ifndef BOOT_LINKER_IRAM_SIZE
#define BOOT_LINKER_IRAM_SIZE          (0x00020000UL)
#endif

#endif /* BOOT_TARGET_CONFIG_H */
