# BootRAM — ARM Cortex-M IAP from external SPI flash (zone/node layout)

Portable **in-application programming** core that:

1. Reads a staged firmware image from **external SPI NOR** (linked sector / zone nodes)
2. Verifies **CRC32**
3. Erases internal application flash
4. Programs the image
5. Runs critical steps from **RAM** (`BOOT_RAM_FUNC` / `.ramfunc`)

MCU-specific code is isolated. The SPI **zone/node** geometry is kept the same.

---

## Layers

| Layer | Files | Who edits? |
|-------|--------|------------|
| **Config** | `boot_config.h` + `platforms/<mcu>/boot_target_config.h` | **You** — memory map per MCU; SPI / transfer in core config |
| **Core** | `boot_core.c` | No — MCU independent |
| **Storage** | `boot_storage.c` | No — same zone/node walk |
| **Port** | `platforms/*/boot_port_*_impl.c` | **You** — SPI, IFLash, CRC, WDT |
| **Hooks** | `boot_hooks` / platform hooks | **You** — start node, expected CRC |
| **Board** | `boot_board_*` | Optional LEDs / progress |
| **HC32 RAM drivers** | `platforms/hc32f460/boot_hc32f460_ram_driver.c` | SPI/CRC/GPIO/EFM/WDT (this MCU) |
| **Facade** | `BootRAM.c` | Only `RAM_ApplySpiFirmware` → core |

Public include: **`boot_api.h`**

Entry: **`boot_core_firmware_update(size, payload_offset, slot)`**  
with `slot` = `BOOT_FW_SLOT_CANDIDATE` or `BOOT_FW_SLOT_GOOD`.  
(HC32 product calls `RAM_ApplySpiFirmware(..., slot)`.)

Flash map / permanent stub: see **`Core/BootStub/README.md`** (stub+meta @0x0..0x3FFF, APP @0x4000..0x78000).

---

## Zone / node layout (unchanged)

```
SPI sector N:
  [ header  BOOT_SPI_SECTOR_HEADER_SIZE ]
  [ payload ... ]
  next node id @ BOOT_SPI_NEXT_NODE_OFFSET (uint16)
```

Defaults (override in `boot_config.h` only if your image layout differs):

- `BOOT_SPI_SECTOR_SIZE` = 4096  
- `BOOT_SPI_SECTOR_HEADER_SIZE` = 192  
- `BOOT_SPI_NEXT_NODE_OFFSET` = 7  

---

## Minimum porting steps

1. Copy this `BootRAM` folder into your project  
2. Edit **`platforms/<mcu>/boot_target_config.h`** (flash + SRAM) and **`boot_config.h`** (SPI / transfer sizes):
   - `BOOT_APP_FLASH_START` / `BOOT_APP_FLASH_END` (in `boot_target_config.h`)
   - `BOOT_APP_FLASH_PAGE_SIZE`
   - `BOOT_TRANSFER_SIZE` (usually = program chunk)
3. Define **one** `BOOT_MCU_*` (or `BOOT_MCU_USER`)  
4. Implement **`boot_port_*`** (start from `platforms/user_template/`)  
5. Implement **hooks**: firmware start node + expected CRC  
6. Place `.ramfunc` in RAM in your linker script  
7. Call `boot_core_firmware_update(...)`

Details: **[PORTING.md](PORTING.md)**

---

## Platform selection

| Define | Meaning |
|--------|---------|
| `BOOT_MCU_HC32F460` | HC32F460 reference (Keil demo define) |
| `BOOT_MCU_STM32_TEMPLATE` | Stub template |
| `BOOT_MCU_STM32F4_GENERIC` | Example STM32F4 port |
| `BOOT_MCU_USER` | Your new MCU (`platforms/user_template`) |

---

## License / note

Product-specific HC32 RAM flash/SPI helpers remain in `BootRAM.c` for this DCU.
New ports should **not** need that file — only `boot_core` + your port/hooks.
