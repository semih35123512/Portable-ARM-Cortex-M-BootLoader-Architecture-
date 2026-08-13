/**
 * Post-SPI staging IAP call site.
 * RAM_ApplySpiFirmware latches meta then runs IAP.
 */
#include "boot_api.h"
#include "platforms/hc32f460/BootRAM.h"

void example_after_spi_image_staged(uint32_t image_size,
                                    uint16_t payload_start,
                                    uint32_t image_crc)
{
    RAM_ApplySpiFirmware(image_size,
                       payload_start,
                       image_crc,
                       BOOT_FW_SLOT_CANDIDATE);
}
