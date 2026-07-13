
ARCH=arm64
CROSS_COMPILER=aarch64-linux-gnu-

# Toolchain
ASSEMBLER = $(CROSS_COMPILER)as
GCC       = $(CROSS_COMPILER)gcc
LINKER    = $(CROSS_COMPILER)ld
OBJDUMP   = $(CROSS_COMPILER)objdump

# Files Inbuilt 'wildcard' to select all .c and .S files
C_SRCS = $(wildcard *.c)
S_SRCS = $(wildcard *.S)

# Substitution reference, make file1.c file2.c to file1.o file2.o 
C_OBJS = $(C_SRCS:.c=.o)
S_OBJS = $(S_SRCS:.S=.o)

#Store All .o into one
OBJS = $(S_OBJS) $(C_OBJS)

TARGET = kernel.elf

# Default target
all: $(TARGET)

# Compile C files
#This is a pattern rule.
#Read it as
#Any .o file depends on the corresponding .c file
# $< means First dependency
# $@ means Target
# combined it becomes  aarch64-linux-gnu-gcc -c -g main.c -o main.o for every file
%.o: %.c
	$(GCC) -c -g $< -o $@

# Assemble .S files
%.o: %.S
	$(ASSEMBLER) -g $< -o $@

# Link traget to objets
#$^  means all dependencies to make single

$(TARGET): $(OBJS)
	$(LINKER) -Map=kernel.map -T linker.ld $^ -o $@
	$(OBJDUMP) -D $(TARGET) > $(TARGET).txt

# Disassemble
dump: $(TARGET)
	$(OBJDUMP) -D $(TARGET) > $(TARGET).txt

# Clean
clean:
	rm -f *.o *.elf *.map *.txt
