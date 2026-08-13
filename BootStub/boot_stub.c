/**
 * @file boot_stub.c
 * @brief Permanent boot stub @0x0 — flag check, GOOD recovery, jump to APP.
 */
#include "boot_stub.h"
#include "boot_mcu_select.h"
#include "boot_config.h"
#include "boot_meta.h"
#include "boot_fw_slot.h"
#include "boot_core.h"

#ifndef BOOT_CMSIS_DEVICE_HEADER
#if defined(BOOT_MCU_HC32F460)
#include "hc32f460.h"
#elif defined(BOOT_MCU_STM32F4_GENERIC) || defined(BOOT_MCU_STM32_TEMPLATE)
#include "stm32f4xx.h"
#else
#include "core_cm4.h"
#endif
#else
#include BOOT_CMSIS_DEVICE_HEADER
#endif

#include <stddef.h>
#include <stdint.h>

typedef void (*boot_stub_fn_t)(void);

static bool boot_stub_sp_in_ram(uint32_t sp)
{
    return (sp >= BOOT_SRAM_START) && (sp <= BOOT_SRAM_END);
}

bool boot_stub_app_image_looks_valid(void)
{
    const uint32_t *app_vectors = (const uint32_t *)(uint32_t)BOOT_APP_FLASH_START;
    uint32_t sp = app_vectors[0];
    uint32_t reset = app_vectors[1];

    if(!boot_stub_sp_in_ram(sp))
    {
        return false;
    }
    if((reset < BOOT_APP_FLASH_START) || (reset >= BOOT_APP_FLASH_END))
    {
        return false;
    }
    if((reset & 1UL) == 0UL)
    {
        return false;
    }
    return true;
}

static void boot_stub_jump_to_app(void)
{
    const uint32_t *app_vectors = (const uint32_t *)(uint32_t)BOOT_APP_FLASH_START;
    uint32_t app_sp = app_vectors[0];
    boot_stub_fn_t app_reset = (boot_stub_fn_t)app_vectors[1];

    __disable_irq();
    SCB->VTOR = BOOT_APP_FLASH_START;
    __DSB();
    __ISB();
    __set_MSP(app_sp);
    app_reset();
    for(;;)
    {
    }
}

static bool boot_stub_should_restore_good(const boot_meta_t *m)
{
    if((m == NULL) || !boot_meta_is_valid(m))
    {
        return !boot_stub_app_image_looks_valid();
    }

    if(m->state == BOOT_META_STATE_UPDATING)
    {
        return true;
    }
    if(m->state == BOOT_META_STATE_FAIL)
    {
        return true;
    }
    if((m->state == BOOT_META_STATE_PROGRAMMED) && (m->boot_ok == 0U))
    {
        if(!boot_stub_app_image_looks_valid())
        {
            return true;
        }
        if(m->boot_attempts >= 3U)
        {
            return true;
        }
        return false;
    }
    if(!boot_stub_app_image_looks_valid())
    {
        return true;
    }
    return false;
}

static void boot_stub_restore_good(const boot_meta_t *m)
{
    if(!boot_meta_is_valid(m) || (m->good_size == 0U) ||
       (m->good_start_node == 0U) || (m->good_start_node == 0xFFFFU))
    {
        return;
    }

    (void)boot_meta_begin_update(BOOT_FW_SLOT_GOOD,
                                 m->good_size,
                                 m->good_payload_start,
                                 m->good_start_node,
                                 m->good_crc);
    boot_core_firmware_update(m->good_size, m->good_payload_start, BOOT_FW_SLOT_GOOD);
}

void boot_stub_on_reset(void)
{
    boot_meta_t meta;
    bool have_meta;

    have_meta = boot_meta_read(&meta);

    if(have_meta && boot_meta_is_valid(&meta) &&
       (meta.state == BOOT_META_STATE_PROGRAMMED) && (meta.boot_ok == 0U) &&
       boot_stub_app_image_looks_valid() && (meta.boot_attempts < 3U))
    {
        meta.boot_attempts++;
        (void)boot_meta_write(&meta);
    }

    if(boot_stub_should_restore_good(have_meta ? &meta : NULL))
    {
        if(have_meta)
        {
            boot_stub_restore_good(&meta);
        }
    }

    if(boot_stub_app_image_looks_valid())
    {
        boot_stub_jump_to_app();
    }

    for(;;)
    {
        __NOP();
    }
}
