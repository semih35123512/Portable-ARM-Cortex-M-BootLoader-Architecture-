# Publishing checklist

Before pushing a **public** GitHub release:

1. Confirm `.gitignore` excludes `MDK-ARM/Objects/`, `Listings/`, `*.axf`, `*.map`, `*.o`, `*.crf`, `*.d`, and `private/`.
2. Do **not** commit:
   - `private/osos_product/` (OSOS zone FS, `ososFlash` / `ososRead`, product link stubs)
   - any APP-copied proprietary headers from the DCU product tree
3. Public HC32 reference is only:
   - `BootRAM/platforms/hc32f460/` (port + `board/` + link stubs)
   - portable core under `BootRAM/` + `BootStub/`
4. `boot_hc32f460_driver_adapter.c` must be a **separate** compilation unit (never `#include "*.c"`).
5. Rebuild the Keil target and verify zero errors/warnings.
