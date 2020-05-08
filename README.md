# TinyUF2

[![Build Status](https://github.com/arturo182/tinyuf2/workflows/Build/badge.svg)](https://github.com/arturo182/tinyuf2/actions) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)

TinyUF2 is an open source cross-platform [UF2](https://github.com/Microsoft/uf2) bootloader based on the [TinyUSB](https://github.com/hathach/tinyusb/) library.

## Building

Checkout the code and sync submodules:

	$ git clone https://github.com/arturo182/tinyuf2.git
	$ cd tinyuf2
	$ git submodule update --init --recursive

Specify the board you want to build for:

	$ make BOARD=mimxrt1010_evk

Currently NXP MIMXRT101x, MIMXRT102x and vexriscv (running on ecp5) chips are supported, running on
mimxrt1011_evk, mixmrt1020_evk, versiboard2, pergola and colorlight_5a_75 boards.

## Use

To enter tinyuf2;

* on MIMXRT push the reset button once, then again within the timeframe of 200-500mS.

* on ECP5 it's integration dependent, but generally just hold down the reset button while the board boots.

It might take a few attempts to get entry right, but in all cases when you're in tinyuf2 you will see a LED flashing at 500mS on,
500mS off and a drive TinyUF2 will appear. Drag and drop an appropriately formatted UF2 file onto this drive and, once it's
flashed into the memory, the board will automatically reboot.

## Filesystem Structure

The filesystem is structured as follows;

```
        tinyuf2
           |
           +----hw                    Hardware specific files
           |    |
           |    +---bsp                 Board specific files
           |    |
           |    +---chip                Chip specific (multiple boards)
           |
           +----lib                   External supporting libraries
           |
           +----src                   tinyUF2 specific code
```

A board is selected by means of the BOARD option on the makefile command line. This references a particular `hw/bsp/<BOARD>` directory which then links to a specific chip and variant in the `hw/chip/` directories.  A new board is added under `/hw/bsp/<BOARD>`. It may optionally need a new chip or variant under the `hw/chip` directories or, more normally, it might need new pin options in `hw/chip/<FAMILY>/<GROUP>/pinning_options.h`.

## Adding a new Board

You can add a new board by copying an existing one that is closest to it in the `hw/` directory with something like `cp -a hw/bsp/mimxrt1010_evk hw/bsp/newboard`. Edit `hw/bsp/newboard/board.mk` to reference the appropriate `hw/chip` directory and the `board_config.h` file to adjust settings for the new board.

In general entries in `hw/bsp/newboard/board.mk` should reference existing pinning definitions in the relavent `hw/chip` directory rather than being created afresh.

## Non-standard options

In general non-standard options are frowned upon because it changes the user experience of tinyUF2. However, on occasion these may be needed for specific reasons. The supported options are specified in `hw/bsp/<BOARD>/board_config.h` for a specific board. Not defining any of these options means sensible defaults will be applied;

* `USB_VID`: Vendor ID for the board. Default is 0x239A, courtesy of Adafruit Industries.

* `USB_PID`: PID for the board. Default is 0x0058 which is specifically defined as a generic tinyUF2 boot device. Do not select something else unless you've been allocated a new number by Adafruit.

* `VOLUME_LABEL`: Volume Label for the tinyUF2 drive. Default is `UF2BOOT`.

* `BOARD_TAP_WAIT`: How long to wait for double-tap entry into bootloader before performing regular boot. Default is 500mS. Note that this is not used on ecp5/vexriscv, which comes directly into the tinyuf2 loader, rather than with a delay.

* `BOARD_BLINK_INTERVAL`: Blinking interval while in tinyUF2. Default is 500mS on/off.

* `BOARD_LED_ON_UF2_START`: Should the LED be on or off at UF2 boot? Default is off. Not relevant to ecp5/vexriscv.