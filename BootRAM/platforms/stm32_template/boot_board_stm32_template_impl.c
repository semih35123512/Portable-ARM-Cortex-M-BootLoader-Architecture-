#include "boot_mcu_select.h"
#if defined(BOOT_MCU_STM32_TEMPLATE)

#include "boot_board.h"
#include "boot_ram_attr.h"

BOOT_RAM_FUNC void boot_board_storage_transfer_begin(void) {}
BOOT_RAM_FUNC void boot_board_storage_transfer_end(void) {}
BOOT_RAM_FUNC void boot_board_set_erase_state(void) {}
BOOT_RAM_FUNC void boot_board_progress_tick(void) {}

#endif
