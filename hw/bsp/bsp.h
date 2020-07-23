/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020, Artur Pacholec
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

#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdint.h>
#include <stdbool.h>

#include "board_config.h"
#include "pinning_options.h"

#ifndef VOLUME_LABEL
#define VOLUME_LABEL "UF2BOOT"
#endif

#ifndef INDEX_URL
#define INDEX_URL    ""
#endif

// Generic VID:PID for boards that don't have their own.
// DO NOT copy a VID:PID from an existing board for a new one,
// just allow this fallback to be used...that's specifically
// what it is for.
#ifndef USB_VID
#define USB_VID 0x239A
#endif

#ifndef USB_PID
#define USB_PID 0x0058
#endif

#ifndef BOARD_TAP_WAIT
// How long to wait for tap in mS
#define BOARD_TAP_WAIT 500
#endif

#ifndef BOARD_BLINK_INTERVAL
// Define this for a different blinking interval in mS
#define BOARD_BLINK_INTERVAL 500
#endif

// If you want multiple taps (i.e. more than two)
// define this to the correct value
// #define BOARD_MULTITAP_COUNT 4

// If you want the LED on while in UF2 boot wait mode
// #define BOARD_LED_ON_UF2_START

void board_flash_flush(void);
uint32_t board_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks);
uint32_t board_flash_write_blocks(const uint8_t *src, uint32_t lba, uint32_t num_blocks);
uint32_t board_millis(void);
uint16_t *board_write_serial_number_string_descriptor(uint16_t *desc_str);
void board_reset(void);
void board_led_write(bool state);
void board_init(void);
void board_delay_ms(uint32_t ms);
void board_check_app_start(void);
void board_check_tinyuf2_start(void);

#endif
