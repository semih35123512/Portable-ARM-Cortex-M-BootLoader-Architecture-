/**
 * @file boot_meta.c
 * @brief boot_meta flash page R/W via boot_port_meta_* port hooks.
 */
#include "boot_meta.h"
#include "boot_config.h"
#include "boot_port.h"
#include <string.h>
#include <stdint.h>

#if defined(__GNUC__) || defined(__clang__)
#define BOOT_META_WEAK __attribute__((weak))
#elif defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
#define BOOT_META_WEAK __attribute__((weak))
#else
#define BOOT_META_WEAK
#endif

/* Port may override these for real flash program/erase of the meta page. */
BOOT_META_WEAK bool boot_port_meta_erase_page(void)
{
    return false;
}

BOOT_META_WEAK bool boot_port_meta_program(const uint8_t *data, uint32_t size)
{
    (void)data;
    (void)size;
    return false;
}

static uint32_t boot_meta_crc32_soft(const uint8_t *data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFFUL;
    for(uint32_t i = 0U; i < len; i++)
    {
        crc ^= data[i];
        for(uint8_t b = 0U; b < 8U; b++)
        {
            uint32_t mask = -(crc & 1UL);
            crc = (crc >> 1) ^ (0xEDB88320UL & mask);
        }
    }
    return ~crc;
}

uint32_t boot_meta_calc_crc(const boot_meta_t *m)
{
    boot_meta_t tmp;
    if(m == NULL)
    {
        return 0U;
    }
    tmp = *m;
    tmp.meta_crc = 0U;
    return boot_meta_crc32_soft((const uint8_t *)&tmp, (uint32_t)sizeof(boot_meta_t) - 4U);
}

bool boot_meta_is_valid(const boot_meta_t *m)
{
    if(m == NULL)
    {
        return false;
    }
    if(m->magic != BOOT_META_MAGIC)
    {
        return false;
    }
    if(m->version != BOOT_META_VERSION)
    {
        return false;
    }
    return (boot_meta_calc_crc(m) == m->meta_crc);
}

bool boot_meta_read(boot_meta_t *out)
{
    const boot_meta_t *flash_meta;

    if(out == NULL)
    {
        return false;
    }

    flash_meta = (const boot_meta_t *)(uint32_t)BOOT_META_FLASH_ADDR;
    memcpy(out, flash_meta, sizeof(boot_meta_t));
    return boot_meta_is_valid(out);
}

bool boot_meta_write(const boot_meta_t *in)
{
    boot_meta_t local;

    if(in == NULL)
    {
        return false;
    }

    local = *in;
    local.magic = BOOT_META_MAGIC;
    local.version = BOOT_META_VERSION;
    local.meta_crc = boot_meta_calc_crc(&local);

    if(!boot_port_meta_erase_page())
    {
        return false;
    }
    return boot_port_meta_program((const uint8_t *)&local, (uint32_t)sizeof(local));
}

bool boot_meta_begin_update(boot_fw_slot_t slot,
                            uint32_t image_size,
                            uint16_t payload_start,
                            uint16_t start_node,
                            uint32_t image_crc)
{
    boot_meta_t m;

    if(!boot_meta_read(&m))
    {
        memset(&m, 0, sizeof(m));
        m.magic = BOOT_META_MAGIC;
        m.version = BOOT_META_VERSION;
    }

    m.state = BOOT_META_STATE_UPDATING;
    m.active_slot = (uint32_t)slot;
    m.boot_ok = 0U;

    if(slot == BOOT_FW_SLOT_GOOD)
    {
        m.good_size = image_size;
        m.good_payload_start = payload_start;
        m.good_start_node = start_node;
        m.good_crc = image_crc;
    }
    else
    {
        m.cand_size = image_size;
        m.cand_payload_start = payload_start;
        m.cand_start_node = start_node;
        m.cand_crc = image_crc;
    }

    return boot_meta_write(&m);
}

BOOT_RAM_FUNC bool boot_meta_set_programmed(void)
{
    boot_meta_t m;
    if(!boot_meta_read(&m))
    {
        return false;
    }
    m.state = BOOT_META_STATE_PROGRAMMED;
    m.boot_ok = 0U;
    m.boot_attempts = 0U;
    return boot_meta_write(&m);
}

BOOT_RAM_FUNC bool boot_meta_set_fail(void)
{
    boot_meta_t m;
    if(!boot_meta_read(&m))
    {
        memset(&m, 0, sizeof(m));
        m.magic = BOOT_META_MAGIC;
        m.version = BOOT_META_VERSION;
    }
    m.state = BOOT_META_STATE_FAIL;
    m.boot_ok = 0U;
    return boot_meta_write(&m);
}

bool boot_meta_confirm_boot_ok(void)
{
    boot_meta_t m;
    if(!boot_meta_read(&m))
    {
        memset(&m, 0, sizeof(m));
        m.magic = BOOT_META_MAGIC;
        m.version = BOOT_META_VERSION;
    }
    m.state = BOOT_META_STATE_IDLE;
    m.boot_ok = 1U;
    return boot_meta_write(&m);
}

bool boot_meta_promote_candidate_descriptors(void)
{
    boot_meta_t m;
    if(!boot_meta_read(&m))
    {
        return false;
    }
    if((m.cand_size == 0U) || (m.cand_start_node == 0U) || (m.cand_start_node == 0xFFFFU))
    {
        return false;
    }
#if (BOOT_SPI_FW_MODE == BOOT_SPI_FW_MODE_FIXED)
    /* MODE2: GOOD lives in its own fixed window (APP copies SPI bytes separately). */
    m.good_size = m.cand_size;
    m.good_crc = m.cand_crc;
    m.good_payload_start = m.cand_payload_start;
    m.good_start_node = (uint16_t)BOOT_SPI_FW_GOOD_SECTOR_FIRST;
#else
    m.good_size = m.cand_size;
    m.good_crc = m.cand_crc;
    m.good_payload_start = m.cand_payload_start;
    m.good_start_node = m.cand_start_node;
#endif

    /* Consume candidate so later boots do not re-promote / rewrite meta. */
		
    m.cand_size = 0U;
    m.cand_crc = 0U;
    m.cand_payload_start = 0U;
    m.cand_start_node = 0xFFFFU;

    return boot_meta_write(&m);
}
