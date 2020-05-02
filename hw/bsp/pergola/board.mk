TUF2_CHIP_FAMILY = mimxrt10xx
TUF2_CHIP_MEMBER = mimxrt101x
TUF2_CHIP_VARIANT = MIMXRT1011DAE5A

$(BUILD)/$(BOARD)-firmware-padded.bin: $(BUILD)/$(BOARD)-firmware.bin
	dd if=/dev/zero of=$(BUILD)/$(BOARD)-firmware-padded.bin bs=1 count=1024
	cat $(BUILD)/$(BOARD)-firmware.bin >> $(BUILD)/$(BOARD)-firmware-padded.bin

$(BUILD)/$(BOARD)-firmware-padded.uf2: $(BUILD)/$(BOARD)-firmware-padded.bin
	@echo CREATE $@
	$(PYTHON) $(TOP)/tools/uf2/utils/uf2conv.py -b $(BOARD_FLASH_BASE) -f $(UF2_FAMILY) -c -o $@ $^

padded-bin: $(BUILD)/$(BOARD)-firmware-padded.bin

padded-uf2: $(BUILD)/$(BOARD)-firmware-padded.uf2

