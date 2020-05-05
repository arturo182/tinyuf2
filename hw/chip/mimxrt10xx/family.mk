APP_START_ADDRESS ?= 0x6000C000
BOARD_FLASH_BASE ?= 0x60000000

CFLAGS += \
	-mthumb \
	-mabi=aapcs \
	-mcpu=cortex-m7 \
	-mfloat-abi=hard \
	-mfpu=fpv5-d16 \
	-D__ARMVFP__=0 -D__ARMFPV5__=0 \
	-D__START=main \
	-DCFG_TUSB_MCU=OPT_MCU_MIMXRT10XX \
	-D__STARTUP_CLEAR_BSS

CFLAGS += -Wno-error=unused-parameter

CFLAGS += \
	-DAPP_START_ADDRESS=$(APP_START_ADDRESS) \
	-DBOARD_FLASH_BASE=$(BOARD_FLASH_BASE) \
	-DUF2_FAMILY=0x4FB2D5BD

SRC_C += \
	hw/chip/$(TUF2_CHIP_FAMILY)/family.c \
	hw/chip/$(TUF2_CHIP_FAMILY)/flexspi_nor_flash_ops.c \
	hw/chip/$(TUF2_CHIP_FAMILY)/flexspi_nor_config.c \
	$(MCU_DIR)/xip/fsl_flexspi_nor_boot.c \
	$(MCU_DIR)/project_template/clock_config.c \
	$(MCU_DIR)/drivers/fsl_clock.c \
	$(MCU_DIR)/drivers/fsl_cache.c \
	$(MCU_DIR)/drivers/fsl_gpio.c \
	$(MCU_DIR)/drivers/fsl_flexspi.c \
	$(MCU_DIR)/drivers/fsl_common.c \
	$(MCU_DIR)/drivers/fsl_lpuart.c

INC += \
	$(TINYUSB_PATH)/$(MCU_DIR) \
	$(TINYUSB_PATH)/$(MCU_DIR)/drivers \
	$(TINYUSB_PATH)/$(MCU_DIR)/project_template \
	$(TINYUSB_PATH)/$(MCU_DIR)/../../CMSIS/Include

SRC_C += \
	$(addprefix $(CURRENT_PATH)/, $(wildcard src/*.c))


# For TinyUSB port source
VENDOR = nxp
CHIP_FAMILY = transdimension

VARIANT ?= flash

ifeq ($(VARIANT), ram)
	CFLAGS += \
		-DXIP_EXTERNAL_FLASH=0 \
		-DXIP_BOOT_HEADER_ENABLE=0
else
	CFLAGS += \
		-DXIP_EXTERNAL_FLASH=1 \
		-DXIP_BOOT_HEADER_ENABLE=1
endif
