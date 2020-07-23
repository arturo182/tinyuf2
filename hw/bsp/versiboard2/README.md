To deploy on versiboard2 you will need to use an external programmer such as blackmagic probe or JLink.

If you are using the .bin file in the `release_images` directory then a command like `loadfile versiboard2-tinyuf2.bin 0x60000000` should do the trick.
