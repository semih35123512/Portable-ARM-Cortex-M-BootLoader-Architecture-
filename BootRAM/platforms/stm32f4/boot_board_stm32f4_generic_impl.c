#include "boot_mcu_select.h"
#if defined(BOOT_MCU_STM32F4_GENERIC)

#include "boot_board.h"

void boot_board_storage_transfer_begin(void) {}
void boot_board_storage_transfer_end(void) {}
void boot_board_set_erase_state(void) {}
void boot_board_progress_tick(void) {}

#endif
