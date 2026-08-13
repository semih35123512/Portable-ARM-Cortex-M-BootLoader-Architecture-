# Boot stub + GOOD / CANDIDATE

## SPI firmware read modes (IAP)

Set in `BootRAM/boot_config.h` (`BOOT_SPI_FW_MODE`, default **MODE2**):

| Mode | Macro | Behavior |
|------|--------|----------|
| **MODE1** | `BOOT_SPI_FW_MODE_ZONE` | Zone FS: header + `nextSector` chain (product integration) |
| **MODE2** | `BOOT_SPI_FW_MODE_FIXED` (**default**) | Fixed sector windows; sequential raw read |

MODE2 defaults (1-based sectors, 4 MB / 1024 sectors):
- CANDIDATE: `769..896` (512 KB)
- GOOD: `897..1024` (512 KB)

On healthy boot in MODE2: meta `good_*` → GOOD window start; SPI bytes copied CAND→GOOD.
IAP/stub reads sequential sectors (no zone header). **OTA download must stage the raw bin into the CANDIDATE sector window** when using MODE2.

## Flash map (EEPROM untouched @0x78000)

| Region | Address | Size |
|--------|---------|------|
| **Boot stub + IAP** | `0x00000000` .. `0x00001FFF` | 8 KB |
| **boot_meta** | `0x00002000` .. `0x00003FFF` | 8 KB |
| **APP** | `0x00004000` .. `0x00077FFF` | 464 KB |
| **EEPROM** | `0x00078000` .. | **unchanged** |

IAP executes from **stub flash**. Former 8 KB `RW_IRAM2` driver image was returned
to APP `.data/.bss` (fixes L6406E ~0x11c8). IAP LED slider trimmed so page0 fits.

## Reset flow

1. Stub @0 → meta / APP check  
2. Recover from SPI **GOOD** if needed  
3. Jump APP (`VTOR=0x4000`)  
4. `main`: `boot_fw_on_app_healthy()` → BOOT_OK + GOOD zone commit
