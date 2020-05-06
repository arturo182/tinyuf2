LITEX_DIR    ?= $(shell realpath ~/Develop/ulx3s/litex/litex/litex/soc/software/)
SOC_DIR      ?= $(shell realpath ~/Develop/ulx3s/litex/litex-boards/litex_boards/targets/soc_basesoc_colorlight_5a_75b/software)

SRC_S += $(TOP)/hw/chip/$(TUF2_CHIP_FAMILY)/$(TUF2_CHIP_MEMBER)/crt0-vexriscv-rom.S

LD_FILE = -T$(TOP)/hw/chip/$(TUF2_CHIP_FAMILY)/$(TUF2_CHIP_MEMBER)/linker-rom.ld  -L$(SOC_DIR)/include -L$(LITEX_DIR)


CROSS_COMPILE ?= riscv64-unknown-elf-
