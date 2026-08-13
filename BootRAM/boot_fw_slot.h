/**
 * @file boot_fw_slot.h
 * @brief SPI firmware slot selection for IAP (candidate vs last-known-good).
 */
#ifndef BOOT_FW_SLOT_H
#define BOOT_FW_SLOT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * BOOT_FW_SLOT_CANDIDATE — staged OTA image
 * BOOT_FW_SLOT_GOOD      — last confirmed image
 */
typedef enum
{
    BOOT_FW_SLOT_CANDIDATE = 0,
    BOOT_FW_SLOT_GOOD      = 1
} boot_fw_slot_t;

#ifdef __cplusplus
}
#endif

#endif /* BOOT_FW_SLOT_H */
