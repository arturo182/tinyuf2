# Program to RAM

Clone and build https://github.com/boundarydevices/imx_usb_loader

Build and program the pergola:
```
rm -rf _build
make IMX_USB_LOADER=~/path/to/imx_usb_loader/imx_usb BOARD=pergola_ram ram
```

Now the UF2 bootloader should run in the RAM of the Pergola. See instructions in `pergola` for steps on how to store TinyUF2 on the flash.
