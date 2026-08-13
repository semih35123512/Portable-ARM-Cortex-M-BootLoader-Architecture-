#include "boot_mcu_select.h"
#if defined(BOOT_MCU_HC32F460)

#include "main.h"
#include "BootRAM.h"
#include "boot_board.h"
#include "boot_port.h"
#include "boot_ram_attr.h"
#include "boot_hc32f460_driver.h"

BOOT_RAM_FUNC void boot_board_storage_transfer_begin(void)
{
    (void)boot_hc32f460_port_set_bits(WD_CTRL_GPIO_Port, WD_CTRL_Pin);
}

BOOT_RAM_FUNC void boot_board_storage_transfer_end(void)
{
    (void)boot_hc32f460_port_reset_bits(WD_CTRL_GPIO_Port, WD_CTRL_Pin);
}

BOOT_RAM_FUNC void boot_board_set_erase_state(void)
{
    boot_port_set_erase_leds();
}

BOOT_RAM_FUNC void boot_board_progress_tick(void)
{
    boot_port_slider_lights();
}

#endif
