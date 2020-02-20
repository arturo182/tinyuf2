#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

// ===============================================================================
//
// TinyUF2 Configuration for Versiboard2
//
// ===============================================================================

// Basic Data
// ==========
#define VENDOR_NAME      "Orbcode"
#define PRODUCT_NAME     "Versiboard2"
#define VOLUME_LABEL     "VB2-BOOT"
#define INDEX_URL        ""
#define BOARD_ID         "Versiboard2"
#define BOARD_FLASH_SIZE 0x800000

// Set VID & PID for board in bootloader mode
#define USB_VID 0x239A
#define USB_PID 0x0077


// TinyUF2 Behaviour
// =================
// How long to wait for tap in mS
#define BOARD_TAP_WAIT       500

// Define this for a different blinking interval in mS
#define BOARD_BLINK_INTERVAL 100

// If you want multiple taps (i.e. more than two) define this to the correct value
#define BOARD_MULTITAP_COUNT 4

// If you want the LED on while in UF2 boot wait mode
#define BOARD_LED_ON_UF2_START


// Board Pinning Data
// ==================

// LED used for status indication
#define PIN_LED          IOMUXC_GPIO_EMC_04_GPIO2_IO04
#define PIN_LED_MUX      5U
#define PIN_LED_CFG      0x10B0U
#define LED_GPIO_PORT    GPIO2
#define LED_GPIO_PIN     04
#define LED_STATE_ON     1

// UART
#define UART_PORT        LPUART1
#define PIN_UART_RX      IOMUXC_GPIO_AD_B0_07_LPUART1_RX
#define PIN_UART_RX_MUX  2U
#define PIN_UART_RX_CFG  0x10B0u
#define PIN_UART_TX      IOMUXC_GPIO_AD_B0_06_LPUART1_TX
#define PIN_UART_TX_MUX  2U
#define PIN_UART_TX_CFG  0x10B0u

// Connection to QSPI
#define PIN_SS0          IOMUXC_GPIO_AD_B1_05_FLEXSPI_A_SS0_B
#define PIN_SS0_MUX      1U
#define PIN_SS0_CFG      0x10E1U
#define PIN_DATA1        IOMUXC_GPIO_AD_B1_04_FLEXSPI_A_DATA01
#define PIN_DATA1_MUX    1U
#define PIN_DATA1_CFG    0x10E1U
#define PIN_DATA2        IOMUXC_GPIO_AD_B1_03_FLEXSPI_A_DATA02
#define PIN_DATA2_MUX    1U
#define PIN_DATA2_CFG    0x10E1U
#define PIN_DATA0        IOMUXC_GPIO_AD_B1_02_FLEXSPI_A_DATA00
#define PIN_DATA0_MUX    1U
#define PIN_DATA0_CFG    0x10E1U
#define PIN_SCLK         IOMUXC_GPIO_AD_B1_01_FLEXSPI_A_SCLK
#define PIN_SCLK_MUX     1U
#define PIN_SCLK_CFG     0x10E1U
#define PIN_DATA3        IOMUXC_GPIO_AD_B1_00_FLEXSPI_A_DATA03
#define PIN_DATA3_MUX    1U
#define PIN_DATA3_CFG    0x10E1U
// (No DQS on this board)

#endif
