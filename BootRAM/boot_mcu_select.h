#ifndef BOOT_MCU_SELECT_H
#define BOOT_MCU_SELECT_H

/*
 * Define exactly ONE BOOT_MCU_* from project compiler defines.
 * No implicit default — the target must select a platform.
 */
#if !defined(BOOT_MCU_HC32F460) && !defined(BOOT_MCU_STM32_TEMPLATE) && \
    !defined(BOOT_MCU_STM32F4_GENERIC) && !defined(BOOT_MCU_USER)
#error "No BOOT_MCU_* defined. Set exactly one: BOOT_MCU_HC32F460, BOOT_MCU_STM32F4_GENERIC, BOOT_MCU_STM32_TEMPLATE, or BOOT_MCU_USER."
#endif

#if (defined(BOOT_MCU_HC32F460) && defined(BOOT_MCU_STM32_TEMPLATE)) || \
    (defined(BOOT_MCU_HC32F460) && defined(BOOT_MCU_STM32F4_GENERIC)) || \
    (defined(BOOT_MCU_HC32F460) && defined(BOOT_MCU_USER)) || \
    (defined(BOOT_MCU_STM32_TEMPLATE) && defined(BOOT_MCU_STM32F4_GENERIC)) || \
    (defined(BOOT_MCU_STM32_TEMPLATE) && defined(BOOT_MCU_USER)) || \
    (defined(BOOT_MCU_STM32F4_GENERIC) && defined(BOOT_MCU_USER))
#error "Only one BOOT_MCU_* platform can be selected."
#endif

#endif /* BOOT_MCU_SELECT_H */
