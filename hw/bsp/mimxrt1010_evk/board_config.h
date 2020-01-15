#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#define VENDOR_NAME "NXP"
#define PRODUCT_NAME "MIMXRT1010-EVK"
#define VOLUME_LABEL "UF2BOOT"
#define INDEX_URL ""
#define BOARD_ID "MIMXRT1010-EVK"

#define USB_VID 0x239A
#define USB_PID 0x0077

// The EVK uses 16MB Flash but that would force the MSC
// drive to be larger than 65535 sectors and requires
// additional modifications to the code, we'll take care
// of that in the future. For now 8MB works.
//#define BOARD_FLASH_SIZE 0x1000000
#define BOARD_FLASH_SIZE 0x800000

#endif
