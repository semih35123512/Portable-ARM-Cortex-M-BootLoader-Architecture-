# User MCU template

1. Define `BOOT_MCU_USER` in your IDE.
2. Edit `../boot_config.h` (flash start/end/page size).
3. Fill TODOs in:
   - `boot_port_user_impl.c`
   - `boot_hooks_user_impl.c`
4. Add these `.c` files + `boot_core.c` + `boot_storage.c` to the build.
5. Include `boot_api.h` and call `boot_core_firmware_update()`.

See `../../PORTING.md`.
