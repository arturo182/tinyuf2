#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

// Basic Data
#define VENDOR_NAME  "NXP"
#define PRODUCT_NAME "MIMXRT1010-EVK"
#define BOARD_ID     "MIMXRT1010-EVK"

// VID & PID
#define USB_VID 0x239A
#define USB_PID 0x0077

// The EVK uses 16MB Flash but that would force the MSC
// drive to be larger than 65535 sectors and requires
// additional modifications to the code, we'll take care
// of that in the future. For now 8MB works.
//#define BOARD_FLASH_SIZE 0x1000000
#define BOARD_FLASH_SIZE 0x800000

// LED
#define PIN_LED         IOMUXC_GPIO_11_GPIOMUX_IO11
#define LED_GPIO_PORT   GPIO1
#define LED_GPIO_PIN    11
#define LED_STATE_ON    1

// UART
#define UART_PORT       LPUART1
#define PIN_UART_RX     IOMUXC_GPIO_09_LPUART1_RXD
#define PIN_UART_TX     IOMUXC_GPIO_10_LPUART1_TXD

// QSPI
#define PINNING_QSPI_FLEXSPIA

#endif
