# This is the root makefile. It calls in the following files;
#   1) hw/bsp/ <BOARD> /board.mk
#         (Sets details of the family, member and variant, and compiler to use)
#
#   2) hw/chip/ <FAMILY> / <MEMBER> / <VARIANT>.mk
#         Sets external directories if these were not set on the command line
#         Includes specific files for that variant
#
#   3) hw/chip/ <FAMILY> /family.mk
#         Main build script for specific family(*)         
#
# Note that for step (3) some builds defer the main build process to tinyuf2, in
# which case $(TINYUSB_PATH)/tools/top.mk and $(TINYUSB_PATH)/examples/rules.mk
# perform the build step rather than the family script.

# Force using the local board directory
TOP := $(shell realpath `pwd`)

TINYUSB_PATH ?= $(TOP)/lib/tinyusb

#-------------- Select the board to build for. ------------
BOARD_LIST = $(sort $(subst /.,,$(subst $(TOP)/hw/bsp/,,$(wildcard $(TOP)/hw/bsp/*/.))))

ifeq ($(filter $(BOARD),$(BOARD_LIST)),)
  $(info You must provide a BOARD parameter with 'BOARD=', supported boards are:)
  $(foreach b,$(BOARD_LIST),$(info - $(b)))
  $(error Invalid BOARD specified)
endif

# Handy check parameter function
check_defined = \
    $(strip $(foreach 1,$1, \
    $(call __check_defined,$1,$(strip $(value 2)))))
__check_defined = \
    $(if $(value $1),, \
    $(error Undefined make flag: $1$(if $2, ($2))))

# Build directory
BUILD = _build/build-$(BOARD)

# Board specific define
include $(TOP)/hw/bsp/$(BOARD)/board.mk

#-------------- Cross Compiler  ------------
# Can be set by board, default to ARM GCC
CROSS_COMPILE ?= arm-none-eabi-

CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size
MKDIR = mkdir
SED = sed
CP = cp
RM = rm

#-------------- Source files and compiler flags --------------

# Include all source C in board folder
#SRC_C += hw/bsp/board.c
SRC_C += $(subst $(TOP)/,,$(wildcard $(TOP)/hw/bsp/$(BOARD)/*.c))

# Compiler Flags which are generic across all targets
CFLAGS += \
        -fdata-sections \
        -ffunction-sections \
        -fsingle-precision-constant \
        -fno-strict-aliasing \
        -Wdouble-promotion \
        -Wstrict-prototypes \
        -Wall \
        -Wextra \
        -Werror \
        -Werror-implicit-function-declaration \
        -Wfloat-equal \
        -Wundef \
        -Wshadow \
        -Wwrite-strings \
        -Wsign-compare \
        -Wmissing-format-attribute \
	-Wno-unused-parameter \
        -Wunreachable-code

# This causes lots of warning with nrf5x build due to nrfx code
# CFLAGS += -Wcast-align

# Debugging/Optimization
ifeq ($(DEBUG), 1)
  CFLAGS += -Og -ggdb
else
        CFLAGS += -Os
endif

# TUSB Logging option
ifneq ($(LOG),)
  CFLAGS += -DCFG_TUSB_DEBUG=$(LOG)
endif

UF2_VERSION_BASE = $(shell git describe --always --tags)
$(BUILD)/uf2_version.h: Makefile
	@echo "#define UF2_VERSION_BASE \"$(UF2_VERSION_BASE)\""> $@

OBJ += $(BUILD)/uf2_version.h

include $(TOP)/hw/chip/$(TUF2_CHIP_FAMILY)/$(TUF2_CHIP_MEMBER)/$(TUF2_CHIP_VARIANT).mk
include $(TOP)/hw/chip/$(TUF2_CHIP_FAMILY)/family.mk

ifndef OVERRIDE_TINYUSB_RULES
include $(TINYUSB_PATH)/tools/top.mk
include $(TINYUSB_PATH)/examples/rules.mk
endif

print-%:
	@echo $* is $($*)

