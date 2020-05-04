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

uint32_t reset_millis = 0;

void reset_task(void)
{
  if (!reset_millis)
    return;

  if (board_millis() > reset_millis)
    board_reset();
}

int main(void)
{
    board_check_app_start();
    board_init();
    board_check_tinyuf2_start();
#ifdef reset_task
    dfsdfdsfsd
#endif

    printf("TinyUF2 running (Version " UF2_VERSION_BASE ", Created " __TIME__ " on " __DATE__ "\r\n");

    tusb_init();

    while (1) {
        tud_task();
        hf2_hid_task();
        board_led_blinking_task();
        reset_task();
    }

    return 0;
}
