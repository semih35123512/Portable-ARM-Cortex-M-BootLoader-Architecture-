#include "boot_mcu_select.h"
#include "boot_board.h"

#if !defined(BOOT_MCU_HC32F460) && !defined(BOOT_MCU_STM32_TEMPLATE) && \
    !defined(BOOT_MCU_STM32F4_GENERIC) && !defined(BOOT_MCU_USER)
#error "Unsupported BOOT_MCU_* selection."
#endif
