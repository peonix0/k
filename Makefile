# ===== Toolchain =====
CROSS     ?= aarch64-linux-gnu-
CC        := $(CROSS)gcc
LD        := $(CROSS)ld
OBJCOPY   := $(CROSS)objcopy
OBJDUMP   := $(CROSS)objdump
QEMU      ?= qemu-system-aarch64

PLATFORM  ?= qemu
INC_DIRS := include

# ===== Flags =====
CFLAGS  := -Wall -Wextra -O0 -g -ffreestanding -fno-builtin -fno-stack-protector \
           -nostdlib -mcmodel=small -march=armv8-a
ASFLAGS := $(CFLAGS) -x assembler-with-cpp
LDFLAGS := -T linker.ld

CFLAGS += -DPLATFORM_$(PLATFORM)
CPPFLAGS += $(addprefix -I, $(INC_DIRS))

# ===== Files =====
OBJS = boot/head.o kernel/kmain.o kernel/uart.o kernel/mmu.o

# ===== Targets =====
.PHONY: all clean run runbin dump

all: k.elf k.bin

k.elf: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

k.bin: k.elf
	$(OBJCOPY) -O binary $< $@

boot/head.o: boot/head.S
	$(CC) $(ASFLAGS) -c $< -o $@

kernel/uart.o: kernel/uart.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@


kernel/mmu.o: kernel/mmu.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

kernel/kmain.o: kernel/kmain.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Run using ELF as kernel (some QEMU builds accept this)
run: k.elf
	$(QEMU) -machine virt -cpu cortex-a53 -nographic -kernel k.elf

# Fallback: load raw binary at our link address (matches linker.ld)
runbin: k.bin
	$(QEMU) -machine virt -cpu cortex-a53 -nographic \
		-device loader,file=k.bin,addr=0x40200000 -serial mon:stdio

dump: k.elf
	$(OBJDUMP) -d k.elf | less

clean:
	rm -f $(OBJS) k.elf k.bin

