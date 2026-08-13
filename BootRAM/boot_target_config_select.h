/**
 * @file boot_target_config_select.h
 * @brief Pull MCU memory map from the active platform (not from core).
 */
#ifndef BOOT_TARGET_CONFIG_SELECT_H
#define BOOT_TARGET_CONFIG_SELECT_H

#include "boot_mcu_select.h"

#if defined(BOOT_MCU_HC32F460)
#include "platforms/hc32f460/boot_target_config.h"
#elif defined(BOOT_MCU_STM32F4_GENERIC)
#include "platforms/stm32f4/boot_target_config.h"
#else
#include "platforms/generic/boot_target_config.h"
#endif

#endif /* BOOT_TARGET_CONFIG_SELECT_H */
