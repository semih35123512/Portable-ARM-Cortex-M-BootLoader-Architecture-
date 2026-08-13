/**
 * @file boot_meta.h
 * @brief Persistent IAP / boot-stub flags (lives outside APP erase range).
 *
 * Stored at BOOT_META_FLASH_ADDR (see boot_config.h). Survives APP erase.
 * Boot stub reads this on every reset to decide: jump APP vs restore GOOD.
 */
#ifndef BOOT_META_H
#define BOOT_META_H

#include <stdint.h>
#include <stdbool.h>
#include "boot_fw_slot.h"
#include "boot_ram_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_META_MAGIC              (0xB007U)
#define BOOT_META_VERSION            (1U)

typedef enum
{
    BOOT_META_STATE_IDLE         = 0U, /* normal */
    BOOT_META_STATE_UPDATING     = 1U, /* erase/program in progress — reset here = interrupted */
    BOOT_META_STATE_PROGRAMMED   = 2U, /* flash written; waiting APP BOOT_OK */
    BOOT_META_STATE_FAIL         = 3U  /* explicit failure latch */
} boot_meta_state_t;

typedef struct
{
    uint32_t magic;                 /* BOOT_META_MAGIC */
    uint32_t version;               /* BOOT_META_VERSION */
    uint32_t state;                 /* boot_meta_state_t */
    uint32_t active_slot;           /* boot_fw_slot_t being applied */
    uint32_t boot_ok;               /* 1 = APP confirmed healthy once */
    uint32_t boot_attempts;         /* PROGRAMMED retries before GOOD rollback */

    uint32_t cand_size;
    uint32_t cand_crc;
    uint16_t cand_payload_start;
    uint16_t cand_start_node;

    uint32_t good_size;
    uint32_t good_crc;
    uint16_t good_payload_start;
    uint16_t good_start_node;

    uint32_t meta_crc;              /* CRC32 over preceding fields */
} boot_meta_t;

bool boot_meta_read(boot_meta_t *out);
bool boot_meta_write(const boot_meta_t *in);

/**
 * Prepare meta before erase (cand_size / cand_payload_start / cand_crc or good_*).
 * Called from RAM_ApplySpiFirmware after SPI staging; stub may call directly for GOOD.
 */
bool boot_meta_begin_update(boot_fw_slot_t slot,
                            uint32_t image_size,
                            uint16_t payload_start,
                            uint16_t start_node,
                            uint32_t image_crc);

/** Mark PROGRAMMED (call from RAM path after successful program, before reset). */
BOOT_RAM_FUNC bool boot_meta_set_programmed(void);

/** Mark FAIL. */
BOOT_RAM_FUNC bool boot_meta_set_fail(void);

/** APP calls once after healthy boot — clears update latch, sets boot_ok. */
bool boot_meta_confirm_boot_ok(void);

/** Copy candidate descriptors into good_*, then clear cand_* so promote runs once. */
bool boot_meta_promote_candidate_descriptors(void);

bool boot_meta_is_valid(const boot_meta_t *m);
uint32_t boot_meta_calc_crc(const boot_meta_t *m);

#ifdef __cplusplus
}
#endif

#endif /* BOOT_META_H */
