#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#define VENDOR_NAME "NXP"
#define PRODUCT_NAME "MIMXRT1020-EVK"
#define CHIP_MIMXRT1020
#define VOLUME_LABEL "UF2BOOT"
#define INDEX_URL ""
#define BOARD_ID "MIMXRT1020-EVK"

#define USB_VID 0x239A
#define USB_PID 0x0077

#define LED_PINMUX            IOMUXC_GPIO_AD_B0_05_GPIO1_IO05
#define LED_PORT              GPIO1
#define LED_PIN               05
#define LED_STATE_ON          0

// UART
#define UART_PORT             LPUART1
#define UART_RX_PINMUX        IOMUXC_GPIO_AD_B0_07_LPUART1_RX
#define UART_TX_PINMUX        IOMUXC_GPIO_AD_B0_06_LPUART1_TX

#define PIN_SS0   IOMUXC_GPIO_SD_B1_11_FLEXSPI_A_SS0_B
#define PIN_DATA1 IOMUXC_GPIO_SD_B1_10_FLEXSPI_A_DATA01
#define PIN_DATA2 IOMUXC_GPIO_SD_B1_09_FLEXSPI_A_DATA02
#define PIN_DATA0 IOMUXC_GPIO_SD_B1_08_FLEXSPI_A_DATA00
#define PIN_SCLK  IOMUXC_GPIO_SD_B1_07_FLEXSPI_A_SCLK
#define PIN_DATA3 IOMUXC_GPIO_SD_B1_06_FLEXSPI_A_DATA03
#define PIN_DQS   IOMUXC_GPIO_SD_B1_05_FLEXSPI_A_DQS

#define PIN_MUX_SS0   1U
#define PIN_MUX_DATA1 1U
#define PIN_MUX_DATA2 1U
#define PIN_MUX_DATA0 1U
#define PIN_MUX_SCLK  1U
#define PIN_MUX_DATA3 1U
#define PIN_MUX_DQS   1U

#define PIN_CFG_SS0   0x10E1U
#define PIN_CFG_DATA1 0x10E1U
#define PIN_CFG_DATA2 0x10E1U
#define PIN_CFG_DATA0 0x10E1U
#define PIN_CFG_SCLK  0x10E1U
#define PIN_CFG_DATA3 0x10E1U
#define PIN_CFG_DQS   0x10E1U

// The EVK uses 16MB Flash but that would force the MSC
// drive to be larger than 65535 sectors and requires
// additional modifications to the code, we'll take care
// of that in the future. For now 8MB works.
//#define BOARD_FLASH_SIZE 0x1000000
#define BOARD_FLASH_SIZE 0x800000

#endif
