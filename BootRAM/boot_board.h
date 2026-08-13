#ifndef BOOT_BOARD_H
#define BOOT_BOARD_H

#include "boot_ram_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Optional board UX during IAP — may be empty stubs. */
BOOT_RAM_FUNC void boot_board_storage_transfer_begin(void);
BOOT_RAM_FUNC void boot_board_storage_transfer_end(void);
BOOT_RAM_FUNC void boot_board_set_erase_state(void);
BOOT_RAM_FUNC void boot_board_progress_tick(void);

#ifdef __cplusplus
}
#endif

#endif /* BOOT_BOARD_H */
