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
#define PINNING_LED_GPIO_11_H

// UART
#define PINNING_UART_LPUART1_GPIO_09_10

// QSPI
#define PINNING_QSPI_FLEXSPIA

#endif
