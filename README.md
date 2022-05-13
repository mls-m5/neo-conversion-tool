# neo-conversion-tool

An overly complicated tool to convert text files imported from the uk neo-pi
to swedish charactetrs

## Getting started

### Install neotools

```bash
pip3 install neotools
[or]
sudo pip3 install neotools
```

### Fix permissions

Reference to original documentation

https://github.com/lykahb/neotools#access-denied

Make sure that your user is a member of the plugdev group.

```bash
usermod -a -G plugdev yourusername
```

Then on a debian system. Create a file like `/lib/udev/rules.d/50-alphasmart.rules`

And add the following lines:

```udev
ACTION=="add", SUBSYSTEMS=="usb", ATTRS{idVendor}=="081e", ATTRS{idProduct}=="bd01", MODE="660", GROUP="plugdev"
ACTION=="add", SUBSYSTEMS=="usb", ATTRS{idVendor}=="081e", ATTRS{idProduct}=="bd04", MODE="660", GROUP="plugdev"
```

## Note

Emscripten stuff is not yet implemented.
