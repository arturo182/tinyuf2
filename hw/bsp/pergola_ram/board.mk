TUF2_CHIP_FAMILY = mimxrt10xx
TUF2_CHIP_MEMBER = mimxrt101x
TUF2_CHIP_VARIANT = MIMXRT1011DAE5A

VARIANT = ram
APP_START_ADDRESS = 0x60000000

# Program directly to RAM using the imx USB ROM bootloader.
# Set IMX_USB_LOADER to the path to your imx_usb_loader/imx_usb binary
ram: $(BUILD)/$(BOARD)-firmware.bin
	$(IMX_USB_LOADER) -c ./hw/bsp/pergola_ram
