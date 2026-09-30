---
title: Installation
nav_order: 2
---

# Installation

FluiDez Reader firmware is published on the
[FluiDez Reader releases page](https://github.com/micheljatuba/FluiDez-Reader/releases).
Each release has one firmware file per reader:

| Reader              | Firmware file                        |
| ------------------- | ------------------------------------ |
| Xteink X4 Pro       | `firmware-x4-pro-v<version>.bin`     |
| Xteink X4 Classic   | `firmware-x4-classic-v<version>.bin` |
| Xteink X3 / X4      | `firmware-x3-x4-v<version>.bin`      |
| Seeed Studio Sticky | `firmware-sticky-v<version>.bin`     |

FluiDez Reader is tested only on the Xteink X4 Pro. The other files are built
from the same source and pass the automated checks, but have not been tested on
a device.

> **Use at your own risk.** You install and update FluiDez Reader at your own
> risk. MJ Cloud Tecnologia is not responsible for damage to your reader, data
> loss, or any other problem that results from installing, updating, or using
> the firmware. Charge the battery first and keep the reader on until the
> update finishes.

## Over-the-Air Updates

After FluiDez Reader is installed, `Settings > System > Check for Updates`
downloads the newest FluiDez Reader release for your reader over Wi-Fi. A
release is offered only when its version is newer than the installed one, for
example `v1.6-fluidez9` over `1.6-fluidez8`. Before installing, the reader shows
that updates are installed at your own risk.

> **Known issue in `1.6-fluidez8`:** `Check for Updates` crashes and restarts
> the X4 Pro while it connects to Wi-Fi. The X4 Classic and Sticky, which use
> the same ESP32-S3 processor, may be affected too. If your reader runs
> `1.6-fluidez8`, install `v1.6-fluidez9` or newer with the
> [SD card method](#sd-card-firmware-update); over-the-air updates work again
> from that version on.

## SD Card Firmware Update

Use this method when the reader already runs FluiDez Reader or the CrossInk
firmware it is based on. It also works on readers with USB data transfer
disabled.

1. Download the `firmware-*.bin` for your reader from the
   [releases page](https://github.com/micheljatuba/FluiDez-Reader/releases).
2. Copy the file to the SD card. Any folder works.
3. On the reader, open `Settings > System > SD Card Firmware Update`, select the
   `.bin` file, and confirm.

## USB Drive

On X4 Pro, choose `Home > File Transfer > USB Drive` to expose the SD card to
your computer. Eject the drive from the computer before disconnecting it; the
reader restarts to Home when the drive is safely ejected or the cable is
removed.

## USB Flashing

Use USB flashing for a reader that runs other firmware, or to recover a reader
that no longer starts. Connect the reader to your computer with a USB-C data
cable. If the flashing tool cannot connect, put the reader in download mode as
described by the device manufacturer and try again.

### From Source with PlatformIO

With the [development setup](./development/getting-started.md) installed, build
and upload the environment for your reader: `x4-pro`, `x4-classic`, `default`
(X3/X4), or `sticky`.

```sh
pio run -e x4-pro --target upload
```

PlatformIO writes the bootloader, partition table, and firmware.

### Release File with esptool

Install `esptool`:

```sh
pip3 install esptool
```

Find the reader's serial port:

```sh
# Linux
dmesg | grep tty

# macOS
ls /dev/cu.*
```

On Windows, the port appears as `COM<n>` in Device Manager under
**Ports (COM & LPT)**.

Clear the saved update slot so the reader starts the image you write, then
write the firmware:

```sh
python3 -m esptool --port /dev/ttyACM0 erase_region 0xe000 0x2000
python3 -m esptool --port /dev/ttyACM0 --baud 921600 write_flash 0x10000 /path/to/firmware.bin
```

Replace the port and firmware path with your actual values. This writes only the
application, so use it on a reader that already runs FluiDez Reader. For other
firmware, use PlatformIO.
