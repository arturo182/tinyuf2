MCU_DIR = hw/mcu/nxp/sdk/devices/MIMXRT1011

include hw/chip/$(TUF2_CHIP_FAMILY)/family.mk

CFLAGS += -DCPU_MIMXRT1011DAE5A

SRC_C += $(MCU_DIR)/system_MIMXRT1011.c

SRC_S += $(MCU_DIR)/gcc/startup_MIMXRT1011.S

LD_FILE = ../../hw/chip/$(TUF2_CHIP_FAMILY)/$(TUF2_CHIP_MEMBER)/MIMXRT1011xxxxx_flexspi_nor.ld

