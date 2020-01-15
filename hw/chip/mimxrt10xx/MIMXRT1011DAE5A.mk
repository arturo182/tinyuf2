MCU_DIR = hw/mcu/nxp/sdk/devices/MIMXRT1011

include hw/chip/mimxrt10xx/family.mk

CFLAGS += -DCPU_MIMXRT1011DAE5A

SRC_C += $(MCU_DIR)/system_MIMXRT1011.c

SRC_S += $(MCU_DIR)/gcc/startup_MIMXRT1011.S

LD_FILE = ../../hw/chip/mimxrt10xx/MIMXRT1011xxxxx_flexspi_nor.ld

