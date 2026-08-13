# PORTING.md — BootRAM on another ARM Cortex-M MCU

## Goal

Keep **core + SPI zone/node storage** unchanged.  
Change only **config + port + hooks**.

---

## 1. Edit `boot_config.h` (required)

```c
#define BOOT_APP_FLASH_START      0x08000000UL   /* your app base */
#define BOOT_APP_FLASH_END        0x08080000UL   /* exclusive end */
#define BOOT_APP_FLASH_PAGE_SIZE  0x4000UL       /* erase/program page */

#define BOOT_TRANSFER_SIZE        8192U          /* program chunk */
/* Zone layout — keep defaults unless your staging image differs */
#define BOOT_SPI_SECTOR_SIZE           4096U
#define BOOT_SPI_SECTOR_HEADER_SIZE    192U
#define BOOT_SPI_NEXT_NODE_OFFSET      7U
```

Optional: if the first firmware node id is fixed:

```c
#define BOOT_FW_START_NODE   12U   /* else implement boot_hook_get_firmware_start_node */
```

---

## 2. Select MCU symbol

Compiler define **exactly one**:

- `BOOT_MCU_USER` → use `platforms/user_template/*`
- or an existing sample (`BOOT_MCU_STM32_TEMPLATE`, …)

---

## 3. Implement `boot_port_*` (required)

File: `platforms/user_template/boot_port_user_impl.c`

| API | Responsibility |
|-----|----------------|
| `boot_port_spi_config` | Init SPI + CS (RAM-safe) |
| `boot_port_spi_read` | Read `size` bytes from SPI flash `address` |
| `boot_port_flash_erase_app_area` | Erase `[START, END)` |
| `boot_port_flash_program_page` | Program one `BOOT_TRANSFER_SIZE` chunk |
| `boot_port_crc_*` | CRC32 init / accumulate / get |
| `boot_port_feed_watchdog` | Kick IWDG/WDT |
| `boot_port_fail_and_reset` | Cleanup + `NVIC_SystemReset` |
| `boot_port_disable_irq` | Mask IRQs before IAP |
| `boot_port_get_cfg` | Usually return struct filled from `boot_config.h` |

Mark these with **`BOOT_RAM_FUNC`** and put `.ramfunc` into SRAM in the linker file.

---

## 4. Implement hooks (required)

| Hook | Meaning |
|------|---------|
| `boot_hook_get_firmware_start_node` | First zone node of staged image |
| `boot_hook_get_expected_image_crc` | Expected CRC of binary |
| `boot_hook_on_crc_calculated` | Optional publish of computed CRC |

On the public HC32 reference, hooks read `boot_meta` (and MODE2 sector defaults).
Product zone-FS tables belong in a private integration tree, not this repo.

---

## 5. Board (optional)

`boot_board_*` — LEDs / external WD GPIO around SPI transfers.  
May be empty stubs.

---

## 6. Call site

```c
#include "boot_api.h"

/* After image is fully staged on SPI zone flash: */
boot_core_firmware_update(binary_size, payload_offset_in_first_sector);
```

Does not return on success (resets via `boot_port_fail_and_reset` after programmed verify path).

---

## 7. Files to add to IDE (minimal set)

**Always:**

- `boot_core.c`
- `boot_storage.c`
- `boot_port.c` / `boot_board.c` (selectors)
- your `boot_port_*_impl.c`
- your `boot_board_*_impl.c`
- your hooks unit (or `boot_hooks.c` weak + strong override)

**HC32 product also keeps:** `BootRAM.c` (low-level RAM SPI/EFM used by adapter).

**New MCU:** you typically **do not** need `BootRAM.c`.

---

## 8. Checklist

- [ ] `boot_config.h` map matches linker app region  
- [ ] `.ramfunc` in RAM, not in erased flash  
- [ ] SPI read works before erase  
- [ ] CRC matches host/staging metadata  
- [ ] WDT fed during long erase/program  
- [ ] Zone header/next-node offsets unchanged (or documented if changed)  

---

## Architecture sketch

```
[ Modem / host staging ] --> SPI zone nodes (same layout)
                                      |
                                      v
                              boot_storage.c  (portable)
                                      |
                                      v
                              boot_core.c     (portable, RAM)
                                      |
                    +-----------------+------------------+
                    v                                    v
            boot_port (SPI/IFLash/CRC/WDT)      boot_hooks (node/CRC)
                    ^                                    ^
                    |                                    |
              YOU implement                        YOU implement
```
