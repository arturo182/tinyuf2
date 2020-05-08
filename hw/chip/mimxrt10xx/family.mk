# This file contains elements that are specific to this family of chips.
# Elements that are generic should go into a higher level makefile.
# Elements which are specific to a chip should go into the lower level mk file.

CFLAGS += \
	-mthumb \
	-mabi=aapcs \
	-mcpu=cortex-m7 \
	-mfloat-abi=hard \
	-mfpu=fpv5-d16 \
	-D__ARMVFP__=0 -D__ARMFPV5__=0 \
	-D__START=main \
	-D__STARTUP_CLEAR_BSS

CFLAGS += -Wno-error=unused-parameter

SRC_C += \
	$(MCU_DIR)/xip/fsl_flexspi_nor_boot.c \
	$(MCU_DIR)/project_template/clock_config.c \
	$(MCU_DIR)/drivers/fsl_clock.c \
	$(MCU_DIR)/drivers/fsl_cache.c \
	$(MCU_DIR)/drivers/fsl_gpio.c \
	$(MCU_DIR)/drivers/fsl_flexspi.c \
	$(MCU_DIR)/drivers/fsl_common.c \
	$(MCU_DIR)/drivers/fsl_lpuart.c

INC += \
	-I$(MCU_DIR) \
	-I$(MCU_DIR)/drivers \
	-I$(MCU_DIR)/project_template \
	-I$(MCU_DIR)/../../CMSIS/Include

# For TinyUSB port source
VENDOR      = nxp
CHIP_FAMILY = transdimension

ifeq ($(VARIANT), ram)
	CFLAGS += \
		-DXIP_EXTERNAL_FLASH=0 \
		-DXIP_BOOT_HEADER_ENABLE=0
else
	CFLAGS += \
		-DXIP_EXTERNAL_FLASH=1 \
		-DXIP_BOOT_HEADER_ENABLE=1
endif
