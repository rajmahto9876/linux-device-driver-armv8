# Phase 1: QEMU Installation:
    - sudo apt update
    - sudo apt install qemu-system-arm qemu-system-aarch64 qemu-utils qemu-efi qemu-user-static           qemu-kvm qemu-system-gui

    ## Verify QEMU Installation
qemu-system-arm --version

## See Available Boards
qemu-system-arm -machine help

# Phase 2: Boot Prebuilt Linux 

## Files Are Needed to Boot Linux

+----------------+
| Linux Kernel   |
+----------------+
        +
+----------------+
| Root Filesystem|
+----------------+

### A Linux system requires:
#### Kernel
- zImage
- Image
- uImage

#### RootFS
- rootfs.ext4
- busybox.img
- debian.img

## Prebuilt Kernel

### Create a Kernel
#### Step 1: Find the "virt" Board
qemu-system-arm -machine help | grep virt

#### Step 2: Create an Empty Virtual Machine
qemu-system-arm -M virt -m 512M -nographic

#### Step3: Get the KernelImage.
- wget https://cloud-images.ubuntu.com/jammy/current/unpacked/jammy-server-cloudimg-arm64-vmlinuz-generic
- cp jammy-server-cloudimg-arm64-vmlinuz-generic kernel
- gunzip kernel
- file kernel

	or
- cd /qemu-lab
- git clone https://github.com/torvalds/linux.git
- cd linux
- make ARCH=arm64 defconfig
- make ARCH=arm64 menuconfig

```text
    Check in Menuconfig...
    Device Drivers
 └── Virtio drivers
      [*] Virtio block driver
      [*] PCI driver for virtio devices
      [*] Virtio MMIO driver
------------------------------------
```

- make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc)

-----------------------------------------
Kernel Image:  arch/arm64/boot/Image
-----------------------------------------

### Create a Minimal BusyBox RootFS
#### Install Build Tools
sudo apt update
sudo apt install build-essential gcc-aarch64-linux-gnu libncurses-dev bison flex bc wget

#### Download BusyBox
- mkdir ~/qemu-rootfs
- cd ~/qemu-rootfs

- wget https://busybox.net/downloads/busybox-1.36.1.tar.bz2
- tar -xjf busybox-1.36.1.tar.bz2
- cd busybox-1.36.1

#### Step 3: Configure BusyBox
- make defconfig
- make menuconfig
- Settings
  └── Build static binary (no shared libs)

#### Step 4: Build BusyBox
- make CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc)
- make CROSS_COMPILE=aarch64-linux-gnu- CONFIG_PREFIX=./rootfs install

#### Step 5: Create Required Directories
- cd qemu-rootfs/rootfs
- mkdir -p proc sys dev tmp
- mkdir -p etc/init.d

#### Step 6: Create Init Script
- vi etc/init.d/rcS

Content :

mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs none /dev

echo "BusyBox RootFS Booted"

/bin/sh

- chmod +x etc/init.d/rcS

#### Step 7: Create init Symlink
- ln -s /bin/busybox init
- ls -l init

#### Step 8: Create RootFS Image
- cd qemu-rootfs
- dd if=/dev/zero of=rootfs.ext4 bs=1M count=128
- mkdir mnt
- sudo mount rootfs.ext4 mnt
- sudo cp -r rootfs/* mnt/
- sudo umount mnt


# With Mount Point:
=============================================
## Host directory:
- mkdir shared
- echo "Hello QEMU" > shared/test.txt

## QEMU
- mkdir /mnt/host
- mount -t 9p -o trans=virtio hostshare /mnt/host
- ls /mnt/host

# Start QEMU
========================================

## Without GDB
---------------------
- qemu-system-aarch64 -M virt -cpu cortex-a57 -m 1024 -kernel kernel -drive file=rootfs.ext4,format=raw,if=virtio -fsdev local,id=fsdev0,path=/home/raj-pc/RajDev/github.io/QEMU_Files/qemu-rootfs/host_shared,security_model=none -device virtio-9p-device,fsdev=fsdev0,mount_tag=hostshare -append "root=/dev/vda rw console=ttyAMA0" -nographic

## With DTB
---------------------

### . Extract the running DT
#### Install dtc on Ubuntu:
- sudo apt install device-tree-compiler

#### Inside QEMU:
- dtc -I fs -O dts /proc/device-tree > qemu.dts
if dtc not availbale:
- cp -r /proc/device-tree /mnt/host/

#### Add Your Node:
Append:

test_device {
    compatible = "raj,test-device";
    status = "okay";
};

#### Compile

-dtc -I dts -O dtb qemu.dts -o qemu.dtb

##### Without Graphics
---------------------
- qemu-system-aarch64 -M virt -cpu cortex-a57 -m 1024 -kernel kernel -dtb qemu.dtbo  -drive file=rootfs.ext4,format=raw,if=virtio -fsdev local,id=fsdev0,path=/home/raj-pc/RajDev/github.io/QEMU_Files/qemu-rootfs/host_shared,security_model=none -device virtio-9p-device,fsdev=fsdev0,mount_tag=hostshare -append "root=/dev/vda rw console=ttyAMA0" -nographic

##### Wit Graphics
---------------------
- qemu-system-aarch64 -M virt -cpu cortex-a57 -m 1024 -kernel kernel -dtb qemu.dtbo  -drive file=rootfs.ext4,format=raw,if=virtio -fsdev local,id=fsdev0,path=/home/raj-pc/RajDev/github.io/QEMU_Files/qemu-rootfs/host_shared,security_model=none -device virtio-9p-device,fsdev=fsdev0,mount_tag=hostshare -append "root=/dev/vda rw console=ttyAMA0"

##### With GDB
---------------------
qemu-system-aarch64 -M virt -cpu cortex-a57 -m 1024 -kernel kernel -dtb qemu.dtbo -S -s -drive file=rootfs.ext4,format=raw,if=virtio -fsdev local,id=fsdev0,path=/home/raj-pc/RajDev/github.io/QEMU_Files/qemu-rootfs/host_shared,security_model=none -device virtio-9p-device,fsdev=fsdev0,mount_tag=hostshare -append "root=/dev/vda rw console=ttyAMA0" -nographic

# Quit Qemu:
=============================================

- ctrl+shift+a  x
