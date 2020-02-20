MCU_DIR = hw/mcu/nxp/sdk/devices/MIMXRT1021

include hw/chip/mimxrt10xx/family.mk

CFLAGS += -DCPU_MIMXRT1021DAG5A

SRC_C += $(MCU_DIR)/system_MIMXRT1021.c

SRC_S += $(MCU_DIR)/gcc/startup_MIMXRT1021.S

LD_FILE = ../../hw/chip/mimxrt10xx/MIMXRT1021xxxxx_flexspi_nor.ld

