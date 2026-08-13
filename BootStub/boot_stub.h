/**
 * @file boot_stub.h
 * @brief Permanent boot stub API (vectors at 0x00000000).
 */
#ifndef BOOT_STUB_H
#define BOOT_STUB_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reset entry from boot_stub_startup — does not return on success path.
 */
void boot_stub_on_reset(void);

bool boot_stub_app_image_looks_valid(void);

#ifdef __cplusplus
}
#endif

#endif /* BOOT_STUB_H */
