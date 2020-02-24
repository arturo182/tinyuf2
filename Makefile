TINYUSB_PATH ?= lib/tinyusb

# Force using the local board directory
TOP := $(shell realpath `pwd`)
include $(TINYUSB_PATH)/examples/make.mk

INC += \
	hw \
	hw/bsp \
	hw/bsp/$(BOARD) \
	src \
	lib/tinyusb/tools \
	_build/build-$(BOARD)

SRC_C = \
	$(addprefix $(CURRENT_PATH)/, $(wildcard src/*.c))

CFLAGS += -Wno-unused-parameter

UF2_VERSION_BASE = $(shell git describe --always --tags)
$(BUILD)/uf2_version.h: Makefile
	@echo "#define UF2_VERSION_BASE \"$(UF2_VERSION_BASE)\""> $@

OBJ += $(BUILD)/uf2_version.h

include $(TOP)/hw/chip/$(TUF2_CHIP_FAMILY)/$(TUF2_CHIP_MEMBER)/$(TUF2_CHIP_VARIANT).mk
include $(TINYUSB_PATH)/tools/top.mk
include $(TINYUSB_PATH)/examples/rules.mk
