/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 * Copyright (c) 2020 Artur Pacholec
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

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bsp.h"
#include "tusb.h"
#include "uf2_version.h"
#include "hid.h"

volatile uint32_t blink_interval_ms = BOARD_BLINK_INTERVAL;

uint32_t reset_millis = 0;

void led_blinking_task(void)
{
    static uint32_t start_ms = 0;
    static bool led_state = false;

    if (board_millis() - start_ms < blink_interval_ms)
        return;

    start_ms += blink_interval_ms;

    led_state = !led_state;
    board_led_write(led_state);
}

void reset_task(void)
{
  if (!reset_millis)
    return;

  if (board_millis() > reset_millis)
    board_reset();
}

void file_loading(void)

{
  blink_interval_ms = BOARD_LOADING_INTERVAL;
}

int main(void)
{
    board_check_app_start();
    board_init();

    board_check_tinyuf2_start();

    printf("TinyUF2 running (Version " UF2_VERSION_BASE ", Created " __TIME__ " on " __DATE__ "\r\n");

    tusb_init();

    while (1) {
        tud_task();
        hf2_hid_task();
        led_blinking_task();
        reset_task();
    }

    return 0;
}
