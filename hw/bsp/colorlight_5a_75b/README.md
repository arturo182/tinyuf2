To deploy on colorlight this can be integrated as a replacement bios.

Simply call up the compiled application as the bios file, as follows;

./colorlight_5a_75b.py --revision=7.0m --integrated-rom-size=32000 --integrated-sram-size=8192 --uart-baudrate=115200  --with-debug=usb --csr-csv soc_basesoc_colorlight_5a_75b/csr.csv --integrated-sram-size=16384 --ecppack-bootaddr=0x80000  --integrated-rom-file=tinyuf2/_build/colorlight_5a_75b/colorlight_5a_75b-firmware.bin

...this will then boot by default and spin up a USB drive to copy firmware to.



