/**
 * @file boot_api.h
 * @brief Public include for portable BootRAM (ARM Cortex-M IAP from external SPI flash).
 *
 * Usage:
 *  1. Edit platforms/<mcu>/boot_target_config.h (flash + SRAM map)
 *  2. Edit boot_config.h if SPI sector windows / transfer sizes differ
 *  3. Select BOOT_MCU_* (or copy platforms/user_template)
 *  3. Implement boot_port_* / boot_hooks_* for your MCU
 *  4. After SPI staging: RAM_ApplySpiFirmware(size, payload_start, image_crc, slot)
 *     (latches meta cand/good fields then boot_core_firmware_update)
 *
 * Boot stub (Core/BootStub) owns reset @0x0, restores GOOD on failed/interrupted IAP.
 */
#ifndef BOOT_API_H
#define BOOT_API_H

#include "boot_config.h"
#include "boot_ram_attr.h"
#include "boot_fw_slot.h"
#include "boot_meta.h"
#include "boot_core.h"
#include "boot_port.h"
#include "boot_storage.h"
#include "boot_board.h"
#include "boot_hooks.h"

#endif /* BOOT_API_H */
