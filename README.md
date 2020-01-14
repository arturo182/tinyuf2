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
