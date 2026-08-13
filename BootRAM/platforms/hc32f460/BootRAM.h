#ifndef BOOT_HC32F460_RAM_DECL_H
#define BOOT_HC32F460_RAM_DECL_H

/**
 * HC32F460 RAM driver declarations (types + prototypes).
 * Implementations: boot_hc32f460_ram_driver.c
 */

#include "string.h"
#include <stdbool.h>
#include <stdint.h>
#include "hc32_ll_def.h"
#include "spi_flash.h"
#include "hc32f460.h"
#include "boot_fw_slot.h"
#include "flash_interface.h"
//#include "hc32_ll_efm.h"


#define BANK1_BASLANGIC_ADRESI   	(uint32_t)0x00004000U
#define BANK1_BITIS_ADRESI   		  (uint32_t)0x00078000U    /* stub+meta@0..0x3FFF, APP@0x4000, eeprom@0x78000 */
																					
#define RAM_F460_FLASH_PAGE_SIZE           0x2000  /* 8KB */   // 0x800     /* 2KB */

#define lightNumber	13

#define RAM_EFM_REG_UNLOCK_KEY1             (0x0123UL)
#define RAM_EFM_REG_UNLOCK_KEY2             (0x3210UL)
#define RAM_EFM_REG_LOCK_KEY                (0x0000UL)
#define RAM_EFM_REG_UNLOCK_KEY              (0x00000001u)

#define RAM_WDT_REFRESH_START_KEY                   ((uint16_t)0x0123)
#define RAM_WDT_REFRESH_END_KEY                     ((uint16_t)0x3210)



#define RAM_MEM_ZERO_STRUCT(x)              do {                                   \
                                        memset((void*)&(x), 0L, (sizeof(x)));  \
                                        }while(0)

																				
#define RAM_EFM_CACHE_ALL                   (EFM_FRMC_CRST | EFM_FRMC_CACHE)
#define RAM_REG_LEN                         (32U)
#define RAM_EFM_TIMEOUT                     (HCLK_VALUE / 20000UL)   /* EFM wait read timeout */
#define RAM_EFM_PGM_TIMEOUT                 (HCLK_VALUE / 20000UL)   /* EFM Program timeout max 53us */
#define RAM_EFM_ERASE_TIMEOUT               (HCLK_VALUE / 50UL)      /* EFM Erase timeout max 20ms */
#define RAM_EFM_SEQ_PGM_TIMEOUT             (HCLK_VALUE / 62500UL)   /* EFM Sequence Program timeout max 16us */
																				
typedef enum ram_en_flag_status
{
    RAM_Reset = 0u,
    RAM_Set   = 1u,
} ram_en_flag_status_t, ram_en_int_status_t;


#define RAM_IS_PWC_FCG1_PERIPH(per)                                                \
(   (((per) & (0xF0F00286u)) == (0x00u))  &&                                     \
    ((0x00u) != (per)))



#define RAM_IS_VALID_FLASH_FLAG(flag)                                              \
(   ((flag) == EFM_FLAG_PEWERR)                 ||                             \
    ((flag) == EFM_FLAG_PEPRTERR)               ||                             \
    ((flag) == EFM_FLAG_PGSZERR)                ||                             \
    ((flag) == EFM_FLAG_PGMISMTCH)              ||                             \
    ((flag) == EFM_FLAG_OPTEND)                    ||                             \
    ((flag) == EFM_FLAG_COLERR)                 ||                             \
    ((flag) == EFM_FLAG_RDY))

/*  Parameter validity check for flash clear flag. */
#define RAM_IS_VALID_CLEAR_FLASH_FLAG(flag)                                        \
(   ((flag) == EFM_FLAG_PEWERR)                 ||                             \
    ((flag) == EFM_FLAG_PEPRTERR)               ||                             \
    ((flag) == EFM_FLAG_PGSZERR)                ||                             \
    ((flag) == EFM_FLAG_PGMISMTCH)              ||                             \
    ((flag) == EFM_FLAG_OPTEND)                 ||                             \
    ((flag) == EFM_FLAG_COLERR))

#define RAM_EFM_TIMEOUT                     (HCLK_VALUE / 20000UL)   /* EFM wait read timeout */

#define RAM_IS_VALID_FLASH_ADDR(addr)                                              \
(   ((addr) == 0x00000000u)                      ||                            \
    (((addr) >= 0x00000001u)                     &&                            \
    ((addr) <= 0x0007FFDFu)))
		
#define RAM_IS_VALID_SS_SETUP_DELAY_OPTION(x)                                      \
(   (RAM_SpiSsSetupDelayTypicalSck1 == (x))         ||                             \
    (RAM_SpiSsSetupDelayCustomValue == (x)))

typedef enum ram_en_spi_ss_setup_delay_option
{
    RAM_SpiSsSetupDelayTypicalSck1 = 0u,        ///< SS setup delay 1 SCK
    RAM_SpiSsSetupDelayCustomValue = 1u,        ///< SS setup delay SCKDL register set value
} ram_en_spi_ss_setup_delay_option_t;		
		
typedef enum ram_en_spi_ss_setup_delay
{
    RAM_SpiSsSetupDelaySck1 = 0u,               ///< Spi SS setup delay 1 SCK
    RAM_SpiSsSetupDelaySck2 = 1u,               ///< Spi SS setup delay 2 SCK
    RAM_SpiSsSetupDelaySck3 = 2u,               ///< Spi SS setup delay 3 SCK
    RAM_SpiSsSetupDelaySck4 = 3u,               ///< Spi SS setup delay 4 SCK
    RAM_SpiSsSetupDelaySck5 = 4u,               ///< Spi SS setup delay 5 SCK
    RAM_SpiSsSetupDelaySck6 = 5u,               ///< Spi SS setup delay 6 SCK
    RAM_SpiSsSetupDelaySck7 = 6u,               ///< Spi SS setup delay 7 SCK
    RAM_SpiSsSetupDelaySck8 = 7u,               ///< Spi SS setup delay 8 SCK
} ram_en_spi_ss_setup_delay_t;



#define RAM_IS_VALID_SS_SETUP_DELAY_TIME(x)                                        \
(   (RAM_SpiSsSetupDelaySck1 == (x))                ||                             \
    (RAM_SpiSsSetupDelaySck2 == (x))                ||                             \
    (RAM_SpiSsSetupDelaySck3 == (x))                ||                             \
    (RAM_SpiSsSetupDelaySck4 == (x))                ||                             \
    (RAM_SpiSsSetupDelaySck5 == (x))                ||                             \
    (RAM_SpiSsSetupDelaySck6 == (x))                ||                             \
    (RAM_SpiSsSetupDelaySck7 == (x))                ||                             \
    (RAM_SpiSsSetupDelaySck8 == (x)))

#define RAM_IS_VALID_SS_HOLD_DELAY_OPTION(x)                                       \
(   (RAM_SpiSsHoldDelayTypicalSck1 == (x))          ||                             \
    (RAM_SpiSsHoldDelayCustomValue == (x)))

typedef enum ram_en_spi_ss_hold_delay_option
{
    RAM_SpiSsHoldDelayTypicalSck1 = 0u,         ///< SS hold delay 1 SCK
    RAM_SpiSsHoldDelayCustomValue = 1u,         ///< SS hold delay SSDL register set value
} ram_en_spi_ss_hold_delay_option_t;

#define RAM_IS_VALID_SS_HOLD_DELAY_TIME(x)                                         \
(   (RAM_SpiSsHoldDelaySck1 == (x))                 ||                             \
    (RAM_SpiSsHoldDelaySck2 == (x))                 ||                             \
    (RAM_SpiSsHoldDelaySck3 == (x))                 ||                             \
    (RAM_SpiSsHoldDelaySck4 == (x))                 ||                             \
    (RAM_SpiSsHoldDelaySck5 == (x))                 ||                             \
    (RAM_SpiSsHoldDelaySck6 == (x))                 ||                             \
    (RAM_SpiSsHoldDelaySck7 == (x))                 ||                             \
    (RAM_SpiSsHoldDelaySck8 == (x)))

typedef enum ram_en_spi_ss_hold_delay
{
    RAM_SpiSsHoldDelaySck1 = 0u,                ///< Spi SS hold delay 1 SCK
    RAM_SpiSsHoldDelaySck2 = 1u,                ///< Spi SS hold delay 2 SCK
    RAM_SpiSsHoldDelaySck3 = 2u,                ///< Spi SS hold delay 3 SCK
    RAM_SpiSsHoldDelaySck4 = 3u,                ///< Spi SS hold delay 4 SCK
    RAM_SpiSsHoldDelaySck5 = 4u,                ///< Spi SS hold delay 5 SCK
    RAM_SpiSsHoldDelaySck6 = 5u,                ///< Spi SS hold delay 6 SCK
    RAM_SpiSsHoldDelaySck7 = 6u,                ///< Spi SS hold delay 7 SCK
    RAM_SpiSsHoldDelaySck8 = 7u,                ///< Spi SS hold delay 8 SCK
} ram_en_spi_ss_hold_delay_t;

typedef enum ram_en_spi_ss_interval_time_option
{
    RAM_SpiSsIntervalTypicalSck1PlusPck2 = 0u,  ///< Spi SS interval time 1 SCK plus 2 PCLK1
    RAM_SpiSsIntervalCustomValue         = 1u,  ///< Spi SS interval time NXTDL register set value
} ram_en_spi_ss_interval_time_option_t;

#define RAM_IS_VALID_SS_INTERVAL_TIME_OPTION(x)                                    \
(   (RAM_SpiSsIntervalTypicalSck1PlusPck2 == (x))   ||                             \
    (RAM_SpiSsIntervalCustomValue == (x)))


#define RAM_IS_VALID_SS_INTERVAL_TIME(x)                                           \
(   (RAM_SpiSsIntervalSck1PlusPck2 == (x))          ||                             \
    (RAM_SpiSsIntervalSck2PlusPck2 == (x))          ||                             \
    (RAM_SpiSsIntervalSck3PlusPck2 == (x))          ||                             \
    (RAM_SpiSsIntervalSck4PlusPck2 == (x))          ||                             \
    (RAM_SpiSsIntervalSck5PlusPck2 == (x))          ||                             \
    (RAM_SpiSsIntervalSck6PlusPck2 == (x))          ||                             \
    (RAM_SpiSsIntervalSck7PlusPck2 == (x))          ||                             \
    (RAM_SpiSsIntervalSck8PlusPck2 == (x)))

typedef enum ram_en_spi_ss_interval_time
{
    RAM_SpiSsIntervalSck1PlusPck2 = 0u,         ///< Spi SS interval time 1 SCK plus 2 PCLK1
    RAM_SpiSsIntervalSck2PlusPck2 = 1u,         ///< Spi SS interval time 2 SCK plus 2 PCLK1
    RAM_SpiSsIntervalSck3PlusPck2 = 2u,         ///< Spi SS interval time 3 SCK plus 2 PCLK1
    RAM_SpiSsIntervalSck4PlusPck2 = 3u,         ///< Spi SS interval time 4 SCK plus 2 PCLK1
    RAM_SpiSsIntervalSck5PlusPck2 = 4u,         ///< Spi SS interval time 5 SCK plus 2 PCLK1
    RAM_SpiSsIntervalSck6PlusPck2 = 5u,         ///< Spi SS interval time 6 SCK plus 2 PCLK1
    RAM_SpiSsIntervalSck7PlusPck2 = 6u,         ///< Spi SS interval time 7 SCK plus 2 PCLK1
    RAM_SpiSsIntervalSck8PlusPck2 = 7u,         ///< Spi SS interval time 8 SCK plus 2 PCLK1
} ram_en_spi_ss_interval_time_t;


#define RAM_IS_VALID_SS_VALID_CHANNEL(x)                                           \
(   (RAM_SpiSsValidChannel0 == (x))                 ||                             \
    (RAM_SpiSsValidChannel1 == (x))                 ||                             \
    (RAM_SpiSsValidChannel2 == (x))                 ||                             \
    (RAM_SpiSsValidChannel3 == (x)))

typedef enum ram_en_spi_ss_valid_channel
{
		RAM_SpiSsValidChannel0 = 0u,                ///< Select SS0 valid
		RAM_SpiSsValidChannel1 = 1u,                ///< Select SS1 valid
		RAM_SpiSsValidChannel2 = 2u,                ///< Select SS2 valid
		RAM_SpiSsValidChannel3 = 3u,                ///< Select SS3 valid
} ram_en_spi_ss_valid_channel_t;


#define RAM_IS_VALID_SS_POLARITY(x)                                                \
(   (SpiSsLowValid == (x))                      ||                             \
    (SpiSsHighValid == (x)))

typedef enum ram_en_spi_ss_polarity
{
    RAM_SpiSsLowValid  = 0u,                    ///< SS0~3 signal low level valid
    RAM_SpiSsHighValid = 1u,                    ///< SS0~3 signal high level valid
} ram_en_spi_ss_polarity_t;

#define RAM_IS_VALID_SPI_UNIT(x)                                                   \
(   (CM_SPI1 == (x))                            ||                             \
    (CM_SPI2 == (x))                            ||                             \
    (CM_SPI3 == (x))                            ||                             \
    (CM_SPI4 == (x)))

#define RAM_IS_VALID_READ_DATA_REG_OBJECT(x)                                       \
(   (RAM_SpiReadReceiverBuffer == (x))              ||                             \
    (RAM_SpiReadSendBuffer == (x)))

typedef enum ram_en_spi_read_object
{
    RAM_SpiReadReceiverBuffer = 0u,             ///< Read receive buffer
    RAM_SpiReadSendBuffer     = 1u,             ///< Read send buffer(must be read when TDEF=1)
} ram_en_spi_read_object_t;


#define RAM_IS_VALID_SCK_POLARITY(x)                                               \
(   (RAM_SpiSckIdleLevelLow == (x))                 ||                             \
    (RAM_SpiSckIdleLevelHigh == (x)))


typedef enum ram_en_spi_sck_polarity
{
    RAM_SpiSckIdleLevelLow  = 0u,               ///< SCK is low level when SCK idle
    RAM_SpiSckIdleLevelHigh = 1u,               ///< SCK is high level when SCK idle
} ram_en_spi_sck_polarity_t;

#define RAM_IS_VALID_SCK_PHASE(x)                                                  \
(   (RAM_SpiSckOddSampleEvenChange == (x))          ||                             \
    (RAM_SpiSckOddChangeEvenSample == (x)))

typedef enum ram_en_spi_sck_phase
{
    RAM_SpiSckOddSampleEvenChange = 0u,         ///< SCK Odd edge data sample,even edge data change
    RAM_SpiSckOddChangeEvenSample = 1u,         ///< SCK Odd edge data change,even edge data sample
} ram_en_spi_sck_phase_t;

#define RAM_IS_VALID_CLK_DIV(x)                                                    \
(   (RAM_SpiClkDiv2 == (x))                         ||                             \
    (RAM_SpiClkDiv4 == (x))                         ||                             \
    (RAM_SpiClkDiv8 == (x))                         ||                             \
    (RAM_SpiClkDiv16 == (x))                        ||                             \
    (RAM_SpiClkDiv32 == (x))                        ||                             \
    (RAM_SpiClkDiv64 == (x))                        ||                             \
    (RAM_SpiClkDiv128 == (x))                       ||                             \
    (RAM_SpiClkDiv256 == (x)))

typedef enum ram_en_spi_clk_div
{
    RAM_SpiClkDiv2   = 0u,                      ///< Spi pclk1 division 2
    RAM_SpiClkDiv4   = 1u,                      ///< Spi pclk1 division 4
    RAM_SpiClkDiv8   = 2u,                      ///< Spi pclk1 division 8
    RAM_SpiClkDiv16  = 3u,                      ///< Spi pclk1 division 16
    RAM_SpiClkDiv32  = 4u,                      ///< Spi pclk1 division 32
    RAM_SpiClkDiv64  = 5u,                      ///< Spi pclk1 division 64
    RAM_SpiClkDiv128 = 6u,                      ///< Spi pclk1 division 128
    RAM_SpiClkDiv256 = 7u,                      ///< Spi pclk1 division 256
} ram_en_spi_clk_div_t;

#define RAM_IS_VALID_DATA_LENGTH(x)                                                \
(   (RAM_SpiDataLengthBit4 == (x))                  ||                             \
    (RAM_SpiDataLengthBit5 == (x))                  ||                             \
    (RAM_SpiDataLengthBit6 == (x))                  ||                             \
    (RAM_SpiDataLengthBit7 == (x))                  ||                             \
    (RAM_SpiDataLengthBit8 == (x))                  ||                             \
    (RAM_SpiDataLengthBit9 == (x))                  ||                             \
    (RAM_SpiDataLengthBit10 == (x))                 ||                             \
    (RAM_SpiDataLengthBit11 == (x))                 ||                             \
    (RAM_SpiDataLengthBit12 == (x))                 ||                             \
    (RAM_SpiDataLengthBit13 == (x))                 ||                             \
    (RAM_SpiDataLengthBit14 == (x))                 ||                             \
    (RAM_SpiDataLengthBit15 == (x))                 ||                             \
    (RAM_SpiDataLengthBit16 == (x))                 ||                             \
    (RAM_SpiDataLengthBit20 == (x))                 ||                             \
    (RAM_SpiDataLengthBit24 == (x))                 ||                             \
    (RAM_SpiDataLengthBit32 == (x)))

typedef enum ram_en_spi_data_length
{
    RAM_SpiDataLengthBit4  = 0u,                ///< 4 bits
    RAM_SpiDataLengthBit5  = 1u,                ///< 5 bits
    RAM_SpiDataLengthBit6  = 2u,                ///< 6 bits
    RAM_SpiDataLengthBit7  = 3u,                ///< 7 bits
    RAM_SpiDataLengthBit8  = 4u,                ///< 8 bits
    RAM_SpiDataLengthBit9  = 5u,                ///< 9 bits
    RAM_SpiDataLengthBit10 = 6u,                ///< 10 bits
    RAM_SpiDataLengthBit11 = 7u,                ///< 11 bits
    RAM_SpiDataLengthBit12 = 8u,                ///< 12 bits
    RAM_SpiDataLengthBit13 = 9u,                ///< 13 bits
    RAM_SpiDataLengthBit14 = 10u,               ///< 14 bits
    RAM_SpiDataLengthBit15 = 11u,               ///< 15 bits
    RAM_SpiDataLengthBit16 = 12u,               ///< 16 bits
    RAM_SpiDataLengthBit20 = 13u,               ///< 20 bits
    RAM_SpiDataLengthBit24 = 14u,               ///< 24 bits
    RAM_SpiDataLengthBit32 = 15u,               ///< 32 bits
} ram_en_spi_data_length_t;

#define RAM_IS_VALID_FIRST_BIT_POSITION(x)                                         \
(   (RAM_SpiFirstBitPositionMSB == (x))             ||                             \
    (RAM_SpiFirstBitPositionLSB == (x)))

typedef enum ram_en_spi_first_bit_position
{
    RAM_SpiFirstBitPositionMSB = 0u,            ///< Spi first bit to MSB
    RAM_SpiFirstBitPositionLSB = 1u,            ///< Spi first bit to LSB
} ram_en_spi_first_bit_position_t;

#define RAM_IS_VALID_FRAME_NUMBER(x)                                               \
(   (RAM_SpiFrameNumber1 == (x))                    ||                             \
    (RAM_SpiFrameNumber2 == (x))                    ||                             \
    (RAM_SpiFrameNumber3 == (x))                    ||                             \
    (RAM_SpiFrameNumber4 == (x)))

typedef enum ram_en_spi_frame_number
{
    RAM_SpiFrameNumber1 = 0u,                   ///< 1 frame data
    RAM_SpiFrameNumber2 = 1u,                   ///< 2 frame data
    RAM_SpiFrameNumber3 = 2u,                   ///< 3 frame data
    RAM_SpiFrameNumber4 = 3u,                   ///< 4 frame data
} ram_en_spi_frame_number_t;

#define RAM_IS_VALID_WORK_MODE(x)                                                  \
(   (RAM_SpiWorkMode4Line == (x))                   ||                             \
    (RAM_SpiWorkMode3Line == (x)))

typedef enum ram_en_spi_work_mode
{
    RAM_SpiWorkMode4Line = 0u,                  ///< 4 lines spi work mode
    RAM_SpiWorkMode3Line = 1u,                  ///< 3 lines spi work mode(clock sync running)
} ram_en_spi_work_mode_t;

#define RAM_IS_VALID_COMM_MODE(x)                                                  \
(   (RAM_SpiTransFullDuplex == (x))                 ||                             \
    (RAM_SpiTransOnlySend == (x)))

typedef enum ram_en_spi_trans_mode
{
    RAM_SpiTransFullDuplex = 0u,                ///< Full duplex sync serial communication
    RAM_SpiTransOnlySend   = 1u,                ///< Only send serial communication
} ram_en_spi_trans_mode_t;

#define RAM_IS_VALID_MASTER_SLAVE_MODE(x)                                          \
(   (RAM_SpiModeSlave == (x))                       ||                             \
    (RAM_SpiModeMaster == (x)))

typedef enum ram_en_spi_master_slave_mode
{
    RAM_SpiModeSlave  = 0u,                     ///< Spi slave mode
    RAM_SpiModeMaster = 1u,                     ///< Spi master mode
} ram_en_spi_master_slave_mode_t;

#define RAM_IS_VALID_PARITY_MODE(x)                                                \
(   (RAM_SpiParityEven == (x))                      ||                             \
    (RAM_SpiParityOdd == (x)))

typedef enum ram_en_spi_parity
{
    RAM_SpiParityEven = 0u,                     ///< Select even parity send and receive
    RAM_SpiParityOdd  = 1u,                     ///< Select odd parity send and receive
} ram_en_spi_parity_t;

typedef enum ram_en_functional_state
{
    RAM_DISABLE = 0u,
    RAM_ENABLE  = 1u,
} ram_en_functional_state_t;


#define RAM_IS_FUNCTIONAL_STATE(state)      (((state) == RAM_DISABLE) || ((state) == RAM_ENABLE))


__WEAKDEF void RAM_Ddl_AssertHandler(uint8_t *file, int16_t line);

#define RAM_DDL_ASSERT(x)                                                          \
do{                                                                            \
    ((x) ? (void)0 : RAM_Ddl_AssertHandler((uint8_t *)__FILE__, __LINE__));        \
}while(0)

#define RAM_IS_PWC_FCG0_PERIPH(per)                                                \
(   (((per) & (0x700C3AEEu)) == (0x00u))  &&                                   \
    ((0x00u) != (per)))


#define RAM_ENABLE_FCG0_REG_WRITE()             (M4_MSTP->FCG0PC = 0xa5a50001u)
#define RAM_DISABLE_FCG0_REG_WRITE()            (M4_MSTP->FCG0PC = 0xa5a50000u)

//typedef enum ram_en_spi_flag_type
//{
//    RAM_SpiFlagReceiveBufferFull = 0u,          ///< Receive buffer full flag
//    RAM_SpiFlagSendBufferEmpty   = 1u,          ///< Send buffer empty flag
//    RAM_SpiFlagUnderloadError    = 2u,          ///< Underload error flag
//    RAM_SpiFlagParityError       = 3u,          ///< Parity error flag
//    RAM_SpiFlagModeFaultError    = 4u,          ///< Mode fault error flag
//    RAM_SpiFlagSpiIdle           = 5u,          ///< SPI idle flag
//    RAM_SpiFlagOverloadError     = 6u,          ///< Overload error flag
//} ram_en_spi_flag_type_t;


#define RAM_IS_VALID_CLR_FLAG_TYPE(x)                                              \
(   (RAM_SpiFlagReceiveBufferFull == (x))           ||                             \
    (RAM_SpiFlagSendBufferEmpty == (x))             ||                             \
    (RAM_SpiFlagUnderloadError == (x))              ||                             \
    (RAM_SpiFlagParityError == (x))                 ||                             \
    (RAM_SpiFlagModeFaultError == (x))              ||                             \
    (RAM_SpiFlagOverloadError == (x)))



/*!< Parameter valid check for flag type */
#define RAM_IS_VALID_FLAG_TYPE(x)                                                  \
(   (RAM_SpiFlagReceiveBufferFull == (x))           ||                             \
    (RAM_SpiFlagSendBufferEmpty == (x))             ||                             \
    (RAM_SpiFlagUnderloadError == (x))              ||                             \
    (RAM_SpiFlagParityError == (x))                 ||                             \
    (RAM_SpiFlagModeFaultError == (x))              ||                             \
    (RAM_SpiFlagSpiIdle == (x))                     ||                             \
    (RAM_SpiFlagOverloadError == (x)))


#define RAM_IS_VALID_PORT(x)                                                        \
(   ((x) == GPIO_PORT_A)                              ||                              \
    ((x) == GPIO_PORT_B)                              ||                              \
    ((x) == GPIO_PORT_C)                              ||                              \
    ((x) == GPIO_PORT_D)                              ||                              \
    ((x) == GPIO_PORT_E)                              ||                              \
    ((x) == GPIO_PORT_H))

#define RAM_IS_VALID_FUNC(x)                                                        \
(   ((x) == Func_Gpio)                          ||                              \
    (((x) >= Func_Fcmref)                       &&                              \
    ((x) <= Func_I2s))                          ||                              \
    ((x) == Func_Evnpt)                         ||                              \
    ((x) == Func_Eventout)                      ||                              \
    (((x) >= Func_Usart1_Tx)                    &&                              \
    ((x) <= Func_I2s2_Ck)))

#define RAM_IS_FUNCTIONAL_STATE(state)      (((state) == RAM_DISABLE) || ((state) == RAM_ENABLE))

#define RAM_IS_VALID_PINMODE(x)                                                     \
(   ((x) == Pin_Mode_In)                        ||                              \
    ((x) == Pin_Mode_Out)                       ||                              \
    ((x) == Pin_Mode_Ana))

#define RAM_IS_VALID_PINDRV(x)                                                      \
(   ((x) == Pin_Drv_L)                          ||                              \
    ((x) == Pin_Drv_M)                          ||                              \
    ((x) == Pin_Drv_H))


#define RAM_IS_VALID_PINTYPE(x)                                                     \
(   ((x) == Pin_OType_Cmos)                     ||                              \
    ((x) == Pin_OType_Od))


//#define __RAM_FUNC HAL_StatusTypeDef


#define RAM_FLASH_DUMMY_BYTE_VALUE          0xff
#define RAM_FLASH_BUSY_BIT_MASK             		0x01

		
#define RAM_SPI_UNIT                        (CM_SPI1)
#define RAM_SPI_UNIT_CLOCK                  (FCG1_PERIPH_SPI1)


#define RAM_FLASH_INSTR_WRITE_ENABLE        (0x06u)
#define RAM_FLASH_INSTR_PAGE_PROGRAM        (0x02u)
#define RAM_FLASH_INSTR_STANDARD_READ       (0x03u)
#define RAM_FLASH_INSTR_ERASE_4KB_SECTOR    (0x20u)
#define RAM_FLASH_INSTR_READ_SR1            (0x05u)


#define FLASH_ER_PRG_TIMEOUT         ((uint32_t)0x8000)
#define FLASH_CACHE_ICACHE_ENABLED           0x01
#define FLASH_CACHE_ICACHE_DCACHE_ENABLED    0x02
#define FLASH_CACHE_DCACHE_ENABLED           0x04
#define FLASH_CACHE_DISABLED				 0x08

extern CM_SPI_TypeDef hspi1;
#define SPI_HandleStructure hspi1

/**
 * @brief After FOTA image is written to SPI: update boot_meta then run IAP.
 * @param binaryFileSize       Image byte size → meta cand_size / good_size
 * @param fileBinStartPoint    Payload offset in first SPI sector → cand_payload_start
 * @param imageCrc             Expected image CRC32 → cand_crc / good_crc
 * @param slot                 BOOT_FW_SLOT_CANDIDATE (OTA) or GOOD (recovery)
 */
extern void RAM_ApplySpiFirmware(uint32_t binaryFileSize,
                               uint16_t fileBinStartPoint,
                               uint32_t imageCrc,
                               boot_fw_slot_t slot);
#endif
