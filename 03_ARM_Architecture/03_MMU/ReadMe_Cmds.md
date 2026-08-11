==============================
QEMU CMD:
==============================
qemu-system-aarch64  -M virt -cpu cortex-a53 -nographic -kernel kernel.elf -S  -gdb tcp::1234

-smp 4
Increase the number io cores on QEMU


==============================
GDB Other Terminal:
==============================
- gdb-multiarch kernel.elf

- target remote :1234