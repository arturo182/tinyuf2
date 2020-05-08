MCU_DIR = $(TINYUSB_PATH)/hw/mcu/nxp/sdk/devices/MIMXRT1011

CFLAGS += -DCPU_MIMXRT1011DAE5A

SRC_C += $(MCU_DIR)/system_MIMXRT1011.c

SRC_S += $(MCU_DIR)/gcc/startup_MIMXRT1011.S

ifeq ($(VARIANT), ram)
	LD_FILE = -T$(TOP)/hw/chip/$(TUF2_CHIP_FAMILY)/$(TUF2_CHIP_MEMBER)/MIMXRT1011xxxxx_ram.ld
else
	LD_FILE = -T$(TOP)/hw/chip/$(TUF2_CHIP_FAMILY)/$(TUF2_CHIP_MEMBER)/MIMXRT1011xxxxx_flexspi_nor.ld
endif


