#include "boot_mcu_select.h"
#if defined(BOOT_MCU_HC32F460)

#include "boot_hc32f460_driver.h"

void boot_hc32f460_spi_config(void)
{
    RAM_Spi_Config();
}

bool boot_hc32f460_spi_read(uint8_t *buffer, uint32_t address, uint16_t size)
{
    return RAM_sFLASH_ReadBuffer(buffer, address, size);
}

void boot_hc32f460_led_on(uint8_t port, uint16_t pin)
{
    RAM_Board_LedOn(port, pin);
}

void boot_hc32f460_led_off(uint8_t port, uint16_t pin)
{
    RAM_Board_LedOff(port, pin);
}

int32_t boot_hc32f460_watchdog_feed(void)
{
    return RAM_WDT_FeedDog();
}

void boot_hc32f460_slider_lights(void)
{
    RAM_Slider_Lights();
}

void boot_hc32f460_crc_init(void)
{
    RAM_CalcCRC32Init();
}

uint32_t boot_hc32f460_crc_accumulate_data32(const uint32_t data[], uint32_t word_count)
{
    return RAM_CRC_AccumulateData32(data, word_count);
}

uint32_t boot_hc32f460_crc_get(void)
{
    return RAM_CalcCRC32Get();
}

void boot_hc32f460_wipe_fw_slot_and_reset(boot_fw_slot_t slot)
{
    RAM_WipeFwSlotAndReset(slot);
}

int32_t boot_hc32f460_flash_erase(FLASH_EraseInitTypeDef *erase_init, uint32_t *page_error)
{
    return HAL_FLASHEx_Erase2(erase_init, page_error);
}

int32_t boot_hc32f460_flash_program(uint32_t address, uint32_t data)
{
    return HAL_FLASH_Program2(address, data);
}

int32_t boot_hc32f460_port_set_bits(uint8_t port, uint16_t pin)
{
    return RAM_PORT_SetBits(port, pin);
}

int32_t boot_hc32f460_port_reset_bits(uint8_t port, uint16_t pin)
{
    return RAM_PORT_ResetBits(port, pin);
}

#endif
