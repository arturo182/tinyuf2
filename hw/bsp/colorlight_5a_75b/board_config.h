#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_
#include "mem.h"

// Basic Data
#define VENDOR_NAME      "Orbcode"
#define PRODUCT_NAME     "Colorlight_5a_75b"
#define BOARD_ID         "Colorlight_5a_75b"

// Board flash
#define BOARD_FLASH_SIZE SPIFLASH_SIZE
#define BOARD_FLASH_PAGE_SIZE 256
#include "mem.h"

// LED
#define LED_STATE_ON     0

#endif
