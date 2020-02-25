#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

// Basic Data
#define VENDOR_NAME      "NXP"
#define PRODUCT_NAME     "MIMXRT1020-EVK"
#define BOARD_ID         "MIMXRT1020-EVK"

// Board flash
#define BOARD_FLASH_SIZE 0x800000

// LED
#define PIN_LED          IOMUXC_GPIO_AD_B0_05_GPIO1_IO05
#define LED_GPIO_PORT    GPIO1
#define LED_GPIO_PIN     05
#define LED_STATE_ON     0

// UART
#define UART_PORT        LPUART1
#define PIN_UART_RX      IOMUXC_GPIO_AD_B0_07_LPUART1_RX
#define PIN_UART_TX      IOMUXC_GPIO_AD_B0_06_LPUART1_TX

// QSPI
#define PINNING_QSPI_FLEXSPIA

#endif
