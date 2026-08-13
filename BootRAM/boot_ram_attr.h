/**
 * @file boot_ram_attr.h
 * @brief Portable "run from RAM" attribute for ARM toolchains (GCC / Keil / IAR).
 *
 * IAP erase/program must not execute from the flash bank being erased.
 * Place .ramfunc / RW_IRAM accordingly in your linker scatter / .ld file.
 */
#ifndef BOOT_RAM_ATTR_H
#define BOOT_RAM_ATTR_H

#if defined(BOOT_RAM_FUNC)
/* User override from compiler defines or boot_config.h */
#elif defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
  #ifndef __RAM_FUNC
    #define __RAM_FUNC __attribute__((section(".ramfunc")))
  #endif
  #define BOOT_RAM_FUNC __RAM_FUNC
#elif defined(__ICCARM__)
  #ifndef __RAM_FUNC
    #define __RAM_FUNC __ramfunc
  #endif
  #define BOOT_RAM_FUNC __RAM_FUNC
#elif defined(__GNUC__)
  #ifndef __RAM_FUNC
    #define __RAM_FUNC __attribute__((long_call, section(".ramfunc")))
  #endif
  #define BOOT_RAM_FUNC __RAM_FUNC
#else
  #define BOOT_RAM_FUNC
#endif

#endif /* BOOT_RAM_ATTR_H */
