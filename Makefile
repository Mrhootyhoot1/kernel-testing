CC = gcc
LD = ld
AS = nasm
OBJCOPY = objcopy

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector \
         -fno-asynchronous-unwind-tables -Wall -Wextra -c

LDFLAGS = -m elf_i386 -nostdlib -T linker.ld

# Find source files
SRC := $(shell find . -name '*.c')
ASM_SRC := $(shell find . -name '*.asm')

# Objects
ENTRY_OBJ := kernel_entry.o

ASM_OBJ := $(ASM_SRC:.asm=.o)
C_OBJ := $(SRC:.c=.o)

# Remove ./ prefix created by find
ASM_OBJ := $(patsubst ./%,%,$(ASM_OBJ))
C_OBJ := $(patsubst ./%,%,$(C_OBJ))

# Put kernel entry first, remove duplicate
OBJ := $(ENTRY_OBJ) $(filter-out $(ENTRY_OBJ),$(ASM_OBJ)) $(C_OBJ)

all: disk.img

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# Assemble ELF objects
%.o: %.asm
	$(AS) -f elf32 $< -o $@

# Assemble boot sector
boot.bin: boot.s
	$(AS) -f bin boot.s -o boot.bin

# Link kernel
kernel.elf: $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $@

# Convert ELF to flat binary
kernel.bin: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel.bin

# Create virtual disk
disk.img: boot.bin kernel.bin
	dd if=/dev/zero of=disk.img bs=1M count=10
	dd if=boot.bin of=disk.img bs=512 count=1 conv=notrunc
	dd if=kernel.bin of=disk.img bs=512 seek=1 conv=notrunc

clean:
	rm -f $(OBJ)
	rm -f kernel.elf kernel.bin boot.bin disk.img

.PHONY: all clean