# ARMv8 Architecture & Linux Device Drivers - Complete Learning Repository

## 📚 Overview

This repository is designed to enhance understanding of **ARMv8 architecture** and **Linux device driver development** through practical, hands-on examples.

**Key Focus Areas:**
- ARMv8 64-bit architecture fundamentals
- Linux kernel device driver development
- Platform device framework and Device Tree support
- QEMU-based emulation and testing

---

## 📁 Repository Structure

```
linux-device-driver/
├── 00_Basic/                    # Beginner Level
│   ├── main.c                  # Simple "Hello World" kernel module
│   └── Makefile                # Build configuration
│
├── 01_Character_Driver/        # Intermediate Level
│   ├── main.c                  # Character device driver implementation
│   └── Makefile                # Build configuration
│
├── 02_Platform_driver/         # Advanced Level - ARMv8 Integration
│   ├── main.c                  # Platform driver with Device Tree support
│   ├── Makefile                # Build configuration
│   └── qemu.dts                # Device Tree Source for ARMv8 system
│       .........
|       .........
|       More Codes
|       .........   
|       .........
|
└── Readme.md                   # This file
```
---

## 📖 ARMv8 Architecture Essentials

**Why ARMv8?**
- Industry standard for embedded Linux systems
- 64-bit architecture enabling efficient memory usage
- Wide adoption in servers, IoT, and mobile devices

**Key ARMv8 Concepts in This Repo:**
- **Exception Levels:** EL0 (user applications) → EL1 (kernel code)
- **Registers:** X0-X30 (general purpose), SP (stack), PC (program counter)
- **Memory Model:** Virtual addressing with Translation Lookaside Buffer (TLB)
- **Interrupts:** GIC provides interrupt controller for devices
- **Calling Conventions:** AAPCS64 (ARM Architecture Procedure Call Standard)

---

## 🛠️ Building and Testing

### Prerequisites
- Linux development environment
- ARM cross-compiler (`arm-linux-gnueabihf-gcc`)
- QEMU ARMv8 emulator (`qemu-system-aarch64`)
- Linux kernel headers for ARM

### Build Each Module
```bash
cd 00_Basic && make
cd ../01_Character_Driver && make
cd ../02_Platform_driver && make
```

### Test with QEMU
```bash
#### Compile

dtc -I dts -O dtb qemu.dts -o qemu.dtb

qemu-system-aarch64 -M virt -cpu cortex-a57 -m 1024 -kernel kernel -dtb qemu.dtbo  -drive file=rootfs.ext4,format=raw,if=virtio -fsdev local,id=fsdev0,path=/home/raj-pc/RajDev/github.io/QEMU_Files/qemu-rootfs/host_shared,security_model=none -device virtio-9p-device,fsdev=fsdev0,mount_tag=hostshare -append "root=/dev/vda rw console=ttyAMA0" -nographic

## With GDB
---------------------
qemu-system-aarch64 -M virt -cpu cortex-a57 -m 1024 -kernel kernel -S -s -drive file=rootfs.ext4,format=raw,if=virtio -fsdev local,id=fsdev0,path=/home/raj-pc/RajDev/github.io/QEMU_Files/qemu-rootfs/host_shared,security_model=none -device virtio-9p-device,fsdev=fsdev0,mount_tag=hostshare -append "root=/dev/vda rw console=ttyAMA0" -nographic

# Quit Qemu:
---------------------
ctrl+shift+a  x
```

## ARMv8 Architecture Quick Reference

### Exception Levels (ELs)

| Level | Name | Purpose | Privileges |
|-------|------|---------|-----------|
| EL0 | User | Applications | Limited |
| EL1 | OS/Kernel | Kernel execution | Full OS |
| EL2 | Hypervisor | VM management | Full VM control |
| EL3 | Secure Monitor | Secure world | Highest |

### Register Types

**General Purpose Registers (64-bit):**
```
X0-X30   (31 general-purpose registers)
X31      (Zero register or Stack Pointer depending on context)
```

**Special Registers:**
```
PC       (Program Counter)
SP       (Stack Pointer)
PSTATE   (Program State)
```

### Addressing Model

```
Virtual Address (VA)  → MMU Translation → Physical Address (PA)
64-bit addressing     → Page tables      → Physical memory
```

## Building and Running

### Prerequisites

```bash
# Install ARM64 cross-compiler
sudo apt-get install gcc-aarch64-linux-gnu

# Install device tree compiler
sudo apt-get install device-tree-compiler

# Install QEMU ARM64 support
sudo apt-get install qemu-system-arm

# Install Linux kernel source for ARM64
# (Set KERN_DIR in Makefile)
```

### Build Steps
#### 1. Build Kernel Module

```bash
make
# Compiles main.c → platform.ko (kernel module)
# Cross-compiles for ARM64 using aarch64-linux-gnu-gcc
```

**Build output:**
- platform.ko - Loadable kernel module
- platform.mod.c - Generated module metadata
- Various object files (.o)

#### 2. Clean Build Artifacts

```bash
make clean
```

#### 3. Get Module Information

```bash
make get_info
# Displays module dependencies and metadata
```

### Running on QEMU

```bash
# Start QEMU with custom device tree
qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a57 \
    -m 512 \
    -dtb qemu.dtbo \
    -kernel vmlinuz-arm64 \
    -initrd initramfs.cpio.gz \
    -append "console=ttyAMA0"

# Inside QEMU, load the driver:
insmod platform.ko

# Check kernel logs:
dmesg | grep TEST

# Expected output:
# TEST: Probe called
# start=0x00a000a00 end=0x00a000bff
# device-name=>[test-device]
# buffer-size=>[256]
# TEST: Probe Successful
```

## Understanding the Flow

### 1. System Boot
```
QEMU starts with device tree
    ↓
Kernel parses DTB
    ↓
Kernel identifies devices (virtio_mmio, pmu, etc.)
    ↓
Platform bus prepared for driver matching
```

### 2. Driver Loading
```
insmod platform.ko
    ↓
Kernel registers platform driver
    ↓
Searches device tree for compatible nodes
    ↓
Finds node with compatible = "raj,test-device"
```

### 3. Device Discovery & Probe
```
Compatible match found
    ↓
kernel calls test_probe()
    ↓
Driver extracts resources:
  - Memory addresses (MMIO)
  - Interrupt numbers
  - Custom properties
    ↓
Driver maps hardware to kernel virtual memory
    ↓
Driver ready for operation
```

### 4. Runtime Operation
```
Device tree properties available to driver:
- device-name: Identifies device
- buffer-size: Configuration parameter
- reg: Hardware addresses
- interrupts: IRQ configuration
```

## Common ARMv8-Specific Concepts

### Memory Model
```
64-bit Virtual Addressing
└── Mapped by MMU through page tables
    ├── 4KB pages (or 2MB, 1GB)
    ├── 4-level page table hierarchy
    └── TLB caching for fast translation
```

### Interrupt Handling (GIC - Generic Interrupt Controller)
```
Peripheral IRQ → GIC → CPU (EL1) → Handler
                └─ Priority, routing, enable/disable
```

### Cache Hierarchy
```
L1 Instruction Cache (32-64 KB per core)
L1 Data Cache (32-64 KB per core)
    ↓
L2 Unified Cache (256-512 KB per core)
    ↓
L3 Unified Cache (shared, multiple MB)
```
## Debugging Tips

### Check Module Compilation
```bash
file platform.ko
# Verify it's ARM64 format: "ELF 64-bit LSB relocatable, ARM aarch64"

modinfo platform.ko
# Show module dependencies and exported symbols
```

### Check Device Tree Compilation
```bash
dtc -I dtb -O dts qemu.dtbo
# Decompile DTB back to human-readable DTS for verification
```

### Kernel Logs
```bash
# During probe:
pr_info("TEST: Probe called\n");
pr_info("start=%pa end=%pa\n", &res->start, &res->end);

# View in kernel log:
dmesg | grep TEST
cat /var/log/kern.log
```

### Interactive Debugging
```bash
# Using GDB with QEMU
qemu-system-aarch64 -gdb tcp::1234 -S ...
# In another terminal:
aarch64-linux-gnu-gdb
(gdb) target remote :1234
(gdb) continue
```

## Next Steps & Advanced Topics

1. **Multi-device Support:** Add more devices to device tree
2. **Interrupt Handlers:** Implement `request_irq()` and ISR
3. **Character Device:** Create /dev interface for user-space access
4. **DMA Operations:** Implement DMA for device communication
5. **Power Management:** Add suspend/resume support
6. **Device Overlays:** Dynamic device tree modifications
7. **SMP (Symmetric Multi-Processing):** Multi-core CPU handling

## Architecture Resources

### Documentation
- [ARMv8-A Reference Manual](https://developer.arm.com/documentation/ddi0487/latest/)
- [Device Tree Specification](https://github.com/devicetree-org/devicetree-specification)
- [Linux Kernel Documentation](https://www.kernel.org/doc/html/latest/)

### Related Topics
- Generic Interrupt Controller (GIC) - Interrupt routing
- ARM Trusted Firmware - Secure world execution
- QEMU ARM64 Emulation - Virtual environment
- Bootloader (UEFI, U-Boot) - System initialization

## Configuration Notes

### Key Makefile Variables

```makefile
ARCH=arm64                              # Target architecture
CROSS_COMPILE=aarch64-linux-gnu-        # Cross-compiler prefix
KERN_DIR = /path/to/linux/source        # Linux kernel source location
ccflags-y := -DMODULE_DEBUG             # Compiler flags
```

### Device Tree Compilation Flags

```bash
dtc -@ -I dts -O dtb -o qemu.dtbo qemu.dts
    ↑  Address generation for overlays
    └─ Allows device tree overlays to reference nodes
```

## Troubleshooting

| Issue | Cause | Solution |
|-------|-------|----------|
| `platform.ko not found` | Build failed | Check `make` output for errors |
| `DTB parsing error` | Invalid DTS syntax | Verify qemu.dts with `dtc -I dts` |
| `Device not discovered` | compatible mismatch | Ensure `compatible` matches in DTS and driver |
| `MMIO access fails` | Wrong address | Check `reg` property in device tree |
| `IRQ mismatch` | Interrupt not routed | Verify `interrupts` and `interrupt-parent` |

## Contributing & Extending

To extend this repository:

1. **Create new devices:** Add nodes to qemu.dts
2. **Implement features:** Extend main.c with functionality
3. **Add documentation:** Update README and inline comments
4. **Test thoroughly:** Verify on QEMU before deployment

---
**Platform:** ARMv8 (ARM64)  
**CPU:** ARM Cortex-A57  
**Emulator:** QEMU (virt machine)  
**OS:** Linux Kernel  
**Language:** C + Device Tree Source  
