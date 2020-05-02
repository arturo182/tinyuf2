# Flashing using MSC:

Build using `rm -rf _build; make BOARD=pergola padded-uf2`

Boot and program the Pergola using the steps in `pergola_ram`.

Mount the partition that shows up and copy the uf2 file there:

```
sudo mount /dev/sdX /mnt/usb
sudo cp _build/build-pergola/pergola-firmware-padded.uf2 /mnt/usb/
sync
sudo umount /mnt/usb
```
