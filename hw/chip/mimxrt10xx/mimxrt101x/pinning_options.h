/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020, Dave Marples
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef _PINNING_OPTIONS_H_
#define _PINNING_OPTIONS_H_

// UART Options =====================================

#if defined(PINNING_UART_LPUART1_GPIO_09_10)

#define UART_PORT       LPUART1
#define PIN_UART_RX     IOMUXC_GPIO_09_LPUART1_RXD
#define PIN_UART_RX_MUX 0U
#define PIN_UART_RX_CFG 0x10B0u
#define PIN_UART_TX     IOMUXC_GPIO_10_LPUART1_TXD
#define PIN_UART_TX_MUX 0U
#define PIN_UART_TX_CFG 0x10B0u

#else

#error No UART pinning set

#endif

// LED Options ======================================

#if defined(PINNING_LED_GPIO_11_H)

#define PIN_LED         IOMUXC_GPIO_11_GPIOMUX_IO11
#define PIN_LED_MUX     0U
#define PIN_LED_CFG     0x10B0U
#define LED_GPIO_PORT   GPIO1
#define LED_GPIO_PIN    11
#define LED_STATE_ON    1

#else

#error No LED pinning set

#endif

// QSPI Options =====================================

#if defined(PINNING_QSPI_FLEXSPIA)

#define PIN_SS0   IOMUXC_GPIO_SD_06_FLEXSPI_A_SS0_B
#define PIN_SS0_MUX   1U
#define PIN_SS0_CFG   0x10E1U
#define PIN_DATA1 IOMUXC_GPIO_SD_07_FLEXSPI_A_DATA1
#define PIN_DATA1_MUX 1U
#define PIN_DATA1_CFG 0x10E1U
#define PIN_DATA2 IOMUXC_GPIO_SD_08_FLEXSPI_A_DATA2
#define PIN_DATA2_MUX 1U
#define PIN_DATA2_CFG 0x10E1U
#define PIN_DATA0 IOMUXC_GPIO_SD_09_FLEXSPI_A_DATA0
#define PIN_DATA0_MUX 1U
#define PIN_DATA0_CFG 0x10E1U
#define PIN_SCLK  IOMUXC_GPIO_SD_10_FLEXSPI_A_SCLK
#define PIN_SCLK_MUX  1U
#define PIN_SCLK_CFG  0x10E1U
#define PIN_DATA3 IOMUXC_GPIO_SD_11_FLEXSPI_A_DATA3
#define PIN_DATA3_MUX 1U
#define PIN_DATA3_CFG 0x10E1U
#define PIN_DQS   IOMUXC_GPIO_SD_12_FLEXSPI_A_DQS
#define PIN_DQS_MUX   1U
#define PIN_DQS_CFG   0x10E1U

#elif defined(PINNING_QSPI_FLEXSPIB)

#define PIN_SS0   IOMUXC_GPIO_SD_00_FLEXSPI_B_SS0_B
#define PIN_SS0_MUX   0U
#define PIN_SS0_CFG   0x10E1U
#define PIN_DATA1 IOMUXC_GPIO_SD_01_FLEXSPI_B_DATA01
#define PIN_DATA1_MUX 0U
#define PIN_DATA1_CFG 0x10E1U
#define PIN_DATA2 IOMUXC_GPIO_SD_02_FLEXSPI_B_DATA02
#define PIN_DATA2_MUX 0U
#define PIN_DATA2_CFG 0x10E1U
#define PIN_DATA0 IOMUXC_GPIO_SD_03_FLEXSPI_B_DATA00
#define PIN_DATA0_MUX 0U
#define PIN_DATA0_CFG 0x10E1U
#define PIN_SCLK  IOMUXC_GPIO_SD_13_FLEXSPI_B_SCLK
#define PIN_SCLK_MUX  0U
#define PIN_SCLK_CFG  0x10E1U
#define PIN_DATA3 IOMUXC_GPIO_SD_04_FLEXSPI_B_DATA03
#define PIN_DATA3_MUX 0U
#define PIN_DATA3_CFG 0x10E1U
#define PIN_DQS   IOMUXC_GPIO_SD_14_FLEXSPI_B_DQS
#define PIN_DQS_MUX   1U
#define PIN_DQS_CFG   0x10E1U

#elif defined(PINNING_QSPI_FLEXSPIB_80PIN)

#define PIN_SS0   IOMUXC_GPIO_SD_00_FLEXSPI_B_SS0_B
#define PIN_SS0_MUX   0U
#define PIN_SS0_CFG   0x10E1U
#define PIN_DATA1 IOMUXC_GPIO_SD_01_FLEXSPI_B_DATA01
#define PIN_DATA1_MUX 0U
#define PIN_DATA1_CFG 0x10E1U
#define PIN_DATA2 IOMUXC_GPIO_SD_02_FLEXSPI_B_DATA02
#define PIN_DATA2_MUX 0U
#define PIN_DATA2_CFG 0x10E1U
#define PIN_DATA0 IOMUXC_GPIO_SD_03_FLEXSPI_B_DATA00
#define PIN_DATA0_MUX 0U
#define PIN_DATA0_CFG 0x10E1U
#define PIN_SCLK  IOMUXC_GPIO_SD_13_FLEXSPI_B_SCLK
#define PIN_SCLK_MUX  0U
#define PIN_SCLK_CFG  0x10E1U
#define PIN_DATA3 IOMUXC_GPIO_SD_04_FLEXSPI_B_DATA03
#define PIN_DATA3_MUX 0U
#define PIN_DATA3_CFG 0x10E1U
#else

#error No QSPI pinning set

#endif


#endif
