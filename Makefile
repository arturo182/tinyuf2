# This is the root makefile. It calls in the following files;
#
#   1) hw/bsp/ <BOARD> /board.mk
#         Configuration of the board details.
#         (Sets details of the family, member and variant, and compiler to use)
#
#   2) hw/chip/ <FAMILY> / <MEMBER> / <VARIANT>.mk
#         Configuration of the specific chip details.
#         Sets external directories if these were not set on the command line
#         Includes specific files and link options for that variant
#
#   3) hw/chip/ <FAMILY> /family.mk
#         Configuration of the chip family details.        
#
# In general, things go in this file if they are generic to the whole of tinyufs,
# into the family makefile if they are generic across a family, and the variant
# file for chip specifics.
#
# All of the configuration for a specific board is stored in the board file.

TOP := ./
TINYUSB_PATH ?= $(TOP)/lib/tinyusb

# Set to 1 to get noisy makefile output
V ?= 0

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
BUILD = _build/$(BOARD)

# Get Board identifier and specific defines
include $(TOP)/hw/bsp/$(BOARD)/board.mk

#-------------- Source files generic across all targets --------------

SRC_C += $(subst $(TOP)/,,$(wildcard $(TOP)/hw/bsp/$(BOARD)/*.c))
SRC_C += $(subst $(TOP)/,,$(wildcard $(TOP)/src/*.c))
SRC_C += $(subst $(TOP)/,,$(wildcard $(TOP)/hw/chip/$(TUF2_CHIP_FAMILY)/*.c))

# TinyUSB Stack source
SRC_C += \
	$(TINYUSB_PATH)/src/tusb.c \
	$(TINYUSB_PATH)/src/common/tusb_fifo.c \
	$(TINYUSB_PATH)/src/device/usbd.c \
	$(TINYUSB_PATH)/src/device/usbd_control.c \
	$(TINYUSB_PATH)/src/class/cdc/cdc_device.c \
	$(TINYUSB_PATH)/src/class/dfu/dfu_rt_device.c \
	$(TINYUSB_PATH)/src/class/hid/hid_device.c \
	$(TINYUSB_PATH)/src/class/midi/midi_device.c \
	$(TINYUSB_PATH)/src/class/msc/msc_device.c \
	$(TINYUSB_PATH)/src/class/net/net_device.c \
	$(TINYUSB_PATH)/src/class/usbtmc/usbtmc_device.c \
	$(TINYUSB_PATH)/src/class/vendor/vendor_device.c \
	$(TINYUSB_PATH)/src/portable/$(VENDOR)/$(CHIP_FAMILY)/dcd_$(CHIP_FAMILY).c

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
        -Wunreachable-code \
	-ffreestanding

# Compiler flags for defining UF2 configuration
CFLAGS += \
	-DAPP_START_ADDRESS=$(APP_START_ADDRESS) \
	-DBOARD_FLASH_BASE=$(BOARD_FLASH_BASE) \
	-DUF2_FAMILY=$(UF2_FAMILY) \
	-DCFG_TUSB_MCU=$(TUSB_MCU)

LDFLAGS += $(CFLAGS) -fshort-enums -Wl,$(LD_FILE) -Wl,-Map=$@.map -Wl,-cref -Wl,-gc-sections

# Debugging/Optimization
ifeq ($(DEBUG), 1)
LDFLAGS += specs=rdimon.specs
else
LDFLAGS += -specs=nosys.specs -specs=nano.specs
endif

ASFLAGS += $(CFLAGS)

# Include directories which are generic across all targets
INC += 	-I$(TOP)/hw \
	-I$(TOP)/hw/bsp \
	-I$(TOP)/hw/bsp/$(BOARD) \
	-I$(TOP)/src \
	-I$(BUILD) \
	-I$(TOP)/hw/chip/$(TUF2_CHIP_FAMILY)/$(TUF2_CHIP_MEMBER) \
	-I$(TINYUSB_PATH)/tools \
	-I$(TINYUSB_PATH)/src

# Bring in the chip and family files

include $(TOP)/hw/chip/$(TUF2_CHIP_FAMILY)/$(TUF2_CHIP_MEMBER)/$(TUF2_CHIP_VARIANT).mk
include $(TOP)/hw/chip/$(TUF2_CHIP_FAMILY)/family.mk

# Can be set by board, default to ARM GCC (done after chip and family in case it's set there)
CROSS_COMPILE ?= arm-none-eabi-

LIBS += 

###############################################################################################
# Shouldn't be too much to mess with below here...
###############################################################################################

# Tool selection
CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size
MKDIR = mkdir
SED = sed
CP = cp
RM = rm

# Debugging/Optimization
ifeq ($(DEBUG), 1)
  CFLAGS += -g3 -Og -ggdb
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

# Assembly files can be name with upper case .S, convert it to .s
SRC_S := $(SRC_S:.S=.s)

# Due to GCC LTO bug https://bugs.launchpad.net/gcc-arm-embedded/+bug/1747966
# assembly file should be placed first in linking order
OBJ += $(addprefix $(BUILD)/obj/, $(SRC_S:.s=.o))
OBJ += $(BUILD)/uf2_version.h
OBJ += $(addprefix $(BUILD)/obj/, $(SRC_C:.c=.o))

# Verbose mode
ifeq ("$(V)","1")
QUIET=
else
QUIET=@
endif

# Set all as default goal
.DEFAULT_GOAL := all
all: $(BUILD)/$(BOARD)-firmware.bin $(BUILD)/$(BOARD)-firmware.hex size

uf2: $(BUILD)/$(BOARD)-firmware.uf2

OBJ_DIRS = $(sort $(dir $(OBJ)))
$(OBJ): | $(OBJ_DIRS)
$(OBJ_DIRS):
	$(QUIET)$(MKDIR) -p $@

$(BUILD)/$(BOARD)-firmware.elf: $(OBJ)
	$(QUIET)echo LINK $@
	$(QUIET)$(CC) -o $@ $(LDFLAGS) $^ -Wl,--start-group $(LIBS) -Wl,--end-group -Wl,-Map=$(BUILD)/$(BOARD)-firmware.map

$(BUILD)/$(BOARD)-firmware.bin: $(BUILD)/$(BOARD)-firmware.elf
	$(QUIET)echo CREATE $@
	$(QUIET)$(OBJCOPY) -O binary $^ $@

$(BUILD)/$(BOARD)-firmware.hex: $(BUILD)/$(BOARD)-firmware.elf  
	$(QUIET)echo CREATE $@
	$(QUIET)$(OBJCOPY) -O ihex $^ $@

$(BUILD)/$(BOARD)-firmware.uf2: $(BUILD)/$(BOARD)-firmware.hex
	$(QUIET)echo CREATE $@
	$(PYTHON) $(MFTOP)/tools/uf2/utils/uf2conv.py -f $(UF2_FAMILY) -c -o $@ $^

# We set vpath to point to the top of the tree so that the source files
# can be located. By following this scheme, it allows a single build rule
# to be used to compile all .c files.
vpath %.c . $(MFTOP)
$(BUILD)/obj/%.o: %.c
	$(QUIET)echo CC $(notdir $@)
	$(QUIET)$(CC) $(CFLAGS) $(INC) -c -MD -o $@ $<
	$(QUIET)# The following fixes the dependency file.
	$(QUIET)# See http://make.paulandlesley.org/autodep.html for details.
	$(QUIET)# Regex adjusted from the above to play better with Windows paths, etc.
	$(QUIET)$(CP) $(@:.o=.d) $(@:.o=.P); \
	  $(SED) -e 's/#.*//' -e 's/^.*:  *//' -e 's/ *\\$$//' \
	      -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.d) >> $(@:.o=.P); \
	  $(RM) $(@:.o=.d)

# ASM sources lower case .s
vpath %.s . $(MFTOP)
$(BUILD)/obj/%.o: %.s
	$(QUIET)echo AS $(notdir $@)
	$(QUIET)$(CC) -x assembler-with-cpp $(ASFLAGS) -c -o $@ $<

# ASM sources upper case .S
vpath %.S . $(MFTOP)
$(BUILD)/obj/%.o: %.S
	$(QUIET)echo AS $(notdir $@)
	$(QUIET)$(CC) -x assembler-with-cpp $(ASFLAGS) -c -o $@ $<

size: $(BUILD)/$(BOARD)-firmware.elf
	-$(QUIET)echo ''
	$(QUIET)$(SIZE) $<
	-$(QUIET)echo ''

clean:
	$(QUIET)rm -rf $(BUILD)

$(BUILD)/$(BOARD)-firmware-padded.bin: $(BUILD)/$(BOARD)-firmware.bin
	$(QUIET)dd if=/dev/zero of=$(BUILD)/$(BOARD)-firmware-padded.bin bs=1 count=1024
	$(QUIET)cat $(BUILD)/$(BOARD)-firmware.bin >> $(BUILD)/$(BOARD)-firmware-padded.bin

$(BUILD)/$(BOARD)-firmware-padded.uf2: $(BUILD)/$(BOARD)-firmware-padded.bin
	@echo CREATE $@
	$(QUIET)$(PYTHON) $(TOP)/tools/uf2/utils/uf2conv.py -b $(BOARD_FLASH_BASE) -f $(UF2_FAMILY) -c -o $@ $^

padded-bin: $(BUILD)/$(BOARD)-firmware-padded.bin

padded-uf2: $(BUILD)/$(BOARD)-firmware-padded.uf2

print-%:
	@echo $* is $($*)
