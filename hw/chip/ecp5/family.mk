GIT_VERSION := $(shell git rev-parse --short HEAD)
OVERRIDE_TINYUSB_RULES = 1
CROSS_COMPILE = riscv64-unknown-elf-
UF2_FAMILY = 0x35504345

CDEFINES = -D__vexriscv__ -DNO_FLOAT
CDEFINES += -DGIT_VERSION=u\"$(GIT_VERSION)\" -DUF2_VERSION_BASE=\"$(GIT_VERSION)\" -std=gnu11
CDEFINES += -DUINT16_MAX=65535
CFLAGS += \
	 $(CDEFINES) \
	 -march=rv32im  -mabi=ilp32 \
	 -nostdinc \
	 -nostartfiles \
	 -fno-common \
	 -ffreestanding \
         -fomit-frame-pointer

SRC_C += \
        hw/chip/$(TUF2_CHIP_FAMILY)/family.c \
        hw/chip/$(TUF2_CHIP_FAMILY)/spi.c

INC += \
	-Isrc \
	-Ihw/bsp \
        -I$(LITEX_DIR)/include \
        -I$(LITEX_DIR)/include/base \
        -I$(SOC_DIR)/include \
        -I$(SOC_DIR)/include/generated \
	-I$(SOC_DIR)/include/hw \
	-Ihw/chip/$(TUF2_CHIP_FAMILY)/$(TUF2_CHIP_MEMBER) \
	-Ihw/bsp/$(BOARD) \
	-I$(BUILD) \
	-I$(TINYUSB_PATH)/src

LIBS += -L$(SOC_DIR)/libbase -lbase-nofloat

# For TinyUSB port source                                                                                                          

CFG_TUSB_MCU = OPT_MCU_VALENTYUSB_EPTRI
CFLAGS   += \
         -DBOARD_FLASH_BASE=0x20080000 \
         -DAPP_START_ADDRESS=0x20080000 \
         -DINDEX_URL=$(INDEX_URL) \
	 -DVOLUME_LABEL=\"$(VOLUME_LABEL)\" \
         -DVENDOR_NAME=\"Orbcode\" \
         -DUF2_FAMILY=$(UF2_FAMILY) \
	 -DCFG_TUSB_MCU=$(CFG_TUSB_MCU)

SRC_C += \
         $(TINYUSB_PATH)/src/class/msc/msc_device.c \
	 $(TINYUSB_PATH)/src/class/cdc/cdc_device.c \
	 $(TINYUSB_PATH)/src/class/hid/hid_device.c \
	 $(TINYUSB_PATH)/src/common/tusb_fifo.c \
	 $(TINYUSB_PATH)/src/device/usbd_control.c \
	 $(TINYUSB_PATH)/src/device/usbd.c \
	 $(TINYUSB_PATH)/src/portable/valentyusb/eptri/dcd_eptri.c \
	 $(TINYUSB_PATH)/src/tusb.c

SRC_C += \
        $(addprefix $(MFTOP)/, $(wildcard src/*.c))

# Assembly files can be name with upper case .S, convert it to .s 
SRC_S := $(SRC_S:.S=.s)

# Due to GCC LTO bug https://bugs.launchpad.net/gcc-arm-embedded/+bug/1747966
# assembly file should be placed first in linking order
OBJ += $(addprefix $(BUILD)/obj/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/obj/, $(SRC_C:.c=.o))

# Setup AS Flags starting with CFLAGS contents
ASFLAGS += $(CFLAGS)

# Setup LD Flags starting including CFLAGS contents
LDFLAGS += $(CFLAGS) $(LD_FILE)

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
	rm -rf $(BUILD)
