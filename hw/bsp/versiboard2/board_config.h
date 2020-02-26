#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

// Basic Data
#define VENDOR_NAME      "Orbcode"
#define PRODUCT_NAME     "Versiboard2"
#define BOARD_ID         "Versiboard2"

// Board flash
#define BOARD_FLASH_SIZE 0x800000

// LED
#define PIN_LED          IOMUXC_GPIO_EMC_04_GPIO2_IO04
#define LED_GPIO_PORT    GPIO2
#define LED_GPIO_PIN     04
#define LED_STATE_ON     1

// UART
#define UART_PORT        LPUART1
#define PIN_UART_RX      IOMUXC_GPIO_AD_B0_07_LPUART1_RX
#define PIN_UART_TX      IOMUXC_GPIO_AD_B0_06_LPUART1_TX

// Connection to QSPI
#define PINNING_QSPI_FLEXSPIA_ALTERNATE

#endif
