# This file contains elements that are specific to this family of chips.
# Elements that are generic should go into a higher level makefile.
# Elements which are specific to a chip should go into the lower level mk file.


CFLAGS += \
	 -DUINT16_MAX=65535 \
	 -D__vexriscv__ \
	 -DNO_FLOAT \
	 -march=rv32im \
         -mabi=ilp32 \
	 -nostdlib

SRC_C += 

INC +=  -I$(LITEX_DIR)/include \
	-I$(LITEX_DIR)/include/base \
	-I$(SOC_DIR)/include \
	-I$(SOC_DIR)/include/generated

LIBS += -L$(SOC_DIR)/libbase -lbase-nofloat

# Configuration for TinyUSB
VENDOR = valentyusb
CHIP_FAMILY = eptri
