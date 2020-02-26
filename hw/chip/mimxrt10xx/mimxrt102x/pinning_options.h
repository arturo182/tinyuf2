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

// Default drive and MUX options for the UART if none are
// provided in the board_config.h

#ifndef PIN_UART_RX_MUX
#define PIN_UART_RX_MUX  2U
#endif

#ifndef PIN_UART_RX_CFG
#define PIN_UART_RX_CFG  0x10B0u
#endif

#ifndef PIN_UART_TX_MUX
#define PIN_UART_TX_MUX  2U
#endif

#ifndef PIN_UART_TX_CFG
#define PIN_UART_TX_CFG  0x10B0u
#endif

// LED Options ======================================

// Default drive and MUX options for the LED used for status indication
// if none are provided in the board_config.h

#ifndef PIN_LED_MUX
#define PIN_LED_MUX      0U
#endif

#ifndef PIN_LED_CFG
#define PIN_LED_CFG      0x10B0U
#endif

// QSPI Options =====================================

#if defined(PINNING_QSPI_FLEXSPIA)

#define PIN_SS0          IOMUXC_GPIO_SD_B1_11_FLEXSPI_A_SS0_B
#define PIN_SS0_MUX      1U
#define PIN_SS0_CFG      0x10E1U
#define PIN_DATA1        IOMUXC_GPIO_SD_B1_10_FLEXSPI_A_DATA01
#define PIN_DATA1_MUX    1U
#define PIN_DATA1_CFG    0x10E1U
#define PIN_DATA2        IOMUXC_GPIO_SD_B1_09_FLEXSPI_A_DATA02
#define PIN_DATA2_MUX    1U
#define PIN_DATA2_CFG    0x10E1U
#define PIN_DATA0        IOMUXC_GPIO_SD_B1_08_FLEXSPI_A_DATA00
#define PIN_DATA0_MUX    1U
#define PIN_DATA0_CFG    0x10E1U
#define PIN_SCLK         IOMUXC_GPIO_SD_B1_07_FLEXSPI_A_SCLK
#define PIN_SCLK_MUX     1U
#define PIN_SCLK_CFG     0x10E1U
#define PIN_DATA3        IOMUXC_GPIO_SD_B1_06_FLEXSPI_A_DATA03
#define PIN_DATA3_MUX    1U
#define PIN_DATA3_CFG    0x10E1U
#define PIN_DQS          IOMUXC_GPIO_SD_B1_05_FLEXSPI_A_DQS
#define PIN_DQS_MUX      1U
#define PIN_DQS_CFG      0x10E1U

#elif defined(PINNING_QSPI_FLEXSPIA_ALTERNATE)

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

#else

#error No QSPI pinning set

#endif

#endif
