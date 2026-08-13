/**
 * @file boot_fw_zone.h
 * @brief SPI zone helpers: commit CANDIDATE firmware zone as GOOD.
 */
#ifndef BOOT_FW_ZONE_H
#define BOOT_FW_ZONE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief After healthy boot: confirm meta, promote descriptors, commit CAND→GOOD.
 */
bool boot_fw_on_app_healthy(void);

/**
 * @brief Commit staged candidate image as GOOD (MODE2 SPI copy; MODE1 = product).
 */
bool boot_fw_zone_commit_candidate_as_good(void);

#ifdef __cplusplus
}
#endif

#endif /* BOOT_FW_ZONE_H */
