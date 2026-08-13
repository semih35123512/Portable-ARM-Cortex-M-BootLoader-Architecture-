/**
 * @file board.h
 * @brief Public HC32F460 reference board pins (edit for your PCB).
 *
 * Not product-proprietary — replace with your schematic pinout.
 */
#ifndef BOOT_HC32F460_BOARD_H
#define BOOT_HC32F460_BOARD_H

#include "hc32_ll.h"
#include "hc32_ll_gpio.h"
#include "hc32_ll_def.h"
#include <stdint.h>

typedef enum
{
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
} GPIO_PinState;

/* Progress / status LEDs (reference mapping — change freely) */
#define ROLE1_LED_Port          (GPIO_PORT_C)
#define ROLE1_LED_Pin           (GPIO_PIN_02)
#define ROLE2_LED_Port          (GPIO_PORT_C)
#define ROLE2_LED_Pin           (GPIO_PIN_03)

#define RS485_LED_Port_1        (GPIO_PORT_C)
#define RS485_LED_Pin_1         (GPIO_PIN_04)
#define RS485_LED_Port_2        (GPIO_PORT_C)
#define RS485_LED_Pin_2         (GPIO_PIN_05)

#define RSSI_LED_1_Port         (GPIO_PORT_B)
#define RSSI_LED_1_Pin          (GPIO_PIN_01)
#define RSSI_LED_2_Port         (GPIO_PORT_B)
#define RSSI_LED_2_Pin          (GPIO_PIN_02)
#define RSSI_LED_3_Port         (GPIO_PORT_B)
#define RSSI_LED_3_Pin          (GPIO_PIN_03)
#define RSSI_LED_4_Port         (GPIO_PORT_B)
#define RSSI_LED_4_Pin          (GPIO_PIN_04)
#define RSSI_LED_5_Port         (GPIO_PORT_B)
#define RSSI_LED_5_Pin          (GPIO_PIN_05)

#define SERVER_LED_Port         (GPIO_PORT_B)
#define SERVER_LED_Pin          (GPIO_PIN_06)
#define CLIENT_LED_Port         (GPIO_PORT_B)
#define CLIENT_LED_Pin          (GPIO_PIN_07)

#define LED_GSM_Port            (GPIO_PORT_B)
#define LED_GSM_Pin             (GPIO_PIN_08)

#define LEDR_GPIO_Port          (GPIO_PORT_B)
#define LEDR_Pin                (GPIO_PIN_09)
#define LEDB_GPIO_Port          (GPIO_PORT_B)
#define LEDB_Pin                (GPIO_PIN_10)

#define WD_CTRL_GPIO_Port       (GPIO_PORT_A)
#define WD_CTRL_Pin             (GPIO_PIN_00)

/* External SPI NOR */
#define SF_CS_PORT              (GPIO_PORT_A)
#define SF_CS_PIN               (GPIO_PIN_04)
#define SF_CS_PORT_2            (GPIO_PORT_B)
#define SF_CS_PIN_2             (GPIO_PIN_12)
#define SF_MISO_PORT            (GPIO_PORT_A)
#define SF_MISO_PIN             (GPIO_PIN_05)
#define SF_SCK_PORT             (GPIO_PORT_A)
#define SF_SCK_PIN              (GPIO_PIN_06)
#define SF_MOSI_PORT            (GPIO_PORT_A)
#define SF_MOSI_PIN             (GPIO_PIN_07)

/* Compatibility for sources that still #include "main.h" */
#ifndef BOOT_SIDE_MAIN_H
#define BOOT_SIDE_MAIN_H
#endif

#endif /* BOOT_HC32F460_BOARD_H */
