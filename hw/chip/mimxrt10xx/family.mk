CFLAGS += \
	-mthumb \
	-mabi=aapcs \
	-mcpu=cortex-m7 \
	-mfloat-abi=hard \
	-mfpu=fpv5-d16 \
	-D__ARMVFP__=0 -D__ARMFPV5__=0 \
	-D__START=main \
	-DXIP_EXTERNAL_FLASH=1 \
	-DXIP_BOOT_HEADER_ENABLE=1 \
	-DCFG_TUSB_MCU=OPT_MCU_MIMXRT10XX \
	-D__STARTUP_CLEAR_BSS

CFLAGS += -Wno-error=unused-parameter

CFLAGS += \
	-DAPP_START_ADDRESS=0x6000C000 \
	-DBOARD_FLASH_BASE=0x60000000 \
	-DUF2_FAMILY=0x4FB2D5BD

SRC_C += \
	hw/chip/mimxrt10xx/family.c \
	hw/chip/mimxrt10xx/flexspi_nor_flash_ops.c \
	hw/chip/mimxrt10xx/flexspi_nor_config.c \
	$(MCU_DIR)/xip/fsl_flexspi_nor_boot.c \
	$(MCU_DIR)/project_template/clock_config.c \
	$(MCU_DIR)/drivers/fsl_clock.c \
	$(MCU_DIR)/drivers/fsl_cache.c \
	$(MCU_DIR)/drivers/fsl_gpio.c \
	$(MCU_DIR)/drivers/fsl_flexspi.c \
	$(MCU_DIR)/drivers/fsl_common.c \
	$(MCU_DIR)/drivers/fsl_lpuart.c

INC += \
	$(TOP)/$(MCU_DIR) \
	$(TOP)/$(MCU_DIR)/drivers \
	$(TOP)/$(MCU_DIR)/project_template \
	$(TOP)/$(MCU_DIR)/../../CMSIS/Include \

# For TinyUSB port source
VENDOR = nxp
CHIP_FAMILY = transdimension
