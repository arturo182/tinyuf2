include lib/tinyusb/tools/top.mk
include lib/tinyusb/examples/make.mk

INC += \
	src \
	$(TOP)/hw

SRC_C += \
	$(addprefix $(CURRENT_PATH)/, $(wildcard src/*.c))

CFLAGS += -Wno-unused-parameter

include lib/tinyusb/examples/rules.mk
