#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

// Basic Data
#define VENDOR_NAME "xil.se"
#define PRODUCT_NAME "Pergola UF2 RAM"
#define BOARD_ID "Pergola"

// W25Q128JVSIQ: 128Mb = 16MB
// TODO: Why doesn't 0x1000000 work?
// #define BOARD_FLASH_SIZE 0x1000000
#define BOARD_FLASH_SIZE 0x800000

#define BOARD_FLASH_PAGE_SIZE 256

// LED
#define PIN_LED         IOMUXC_GPIO_03_GPIOMUX_IO03
#define LED_GPIO_PORT   GPIO1
#define LED_GPIO_PIN    3
#define LED_STATE_ON    0

// UART
#define UART_PORT       LPUART1
#define PIN_UART_RX     IOMUXC_GPIO_09_LPUART1_RXD
#define PIN_UART_TX     IOMUXC_GPIO_10_LPUART1_TXD

// QSPI
#define PINNING_QSPI_FLEXSPIA

#endif
