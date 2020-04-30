LITEX_DIR    ?= $(shell realpath ~/Develop/ulx3s/litex/litex/litex/soc/software/)
SOC_DIR      ?= $(shell realpath ~/Develop/ulx3s/litex/litex-boards/litex_boards/targets/soc_basesoc_colorlight_5a_75b/software)

SRC_S += $(LITEX_DIR)/libbase/crt0-vexriscv.S

LD_FILE = -T$(TOP)/hw/chip/$(TUF2_CHIP_FAMILY)/$(TUF2_CHIP_MEMBER)/linker.ld  -L$(SOC_DIR)/include -L$(LITEX_DIR)
