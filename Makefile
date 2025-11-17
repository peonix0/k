# ===== Toolchain =====
CROSS     ?= aarch64-linux-gnu-
CC        := $(CROSS)gcc
LD        := $(CROSS)ld
OBJCOPY   := $(CROSS)objcopy
OBJDUMP   := $(CROSS)objdump
QEMU      ?= qemu-system-aarch64

PLATFORM  ?= qemu
INC_DIRS := include
DEBUG ?= 1

# ===== Flags =====
CFLAGS  := -Wall -Wextra -O0 -g -ffreestanding -fno-builtin -fno-stack-protector \
           -nostdlib -mcmodel=small -march=armv8-a -fno-omit-frame-pointer
ASFLAGS := $(CFLAGS) -x assembler-with-cpp
ASFLAGS += -DPLATFORM_$(PLATFORM)
LDFLAGS := -T linker.ld

# Set DEBUG=1 on the make command line to enable logging
ifeq ($(DEBUG),1)
    CFLAGS += -DDEBUG
endif

CFLAGS += -DPLATFORM_$(PLATFORM)
CPPFLAGS += $(addprefix -I, $(INC_DIRS))

# ===== Files =====
OBJS = boot/head.o boot/vector_el1.o \
       kernel/kmain.o kernel/uart.o kernel/mmu.o kernel/exception.o \
       kernel/generic_timer.o kernel/gic.o kernel/debug.o \
       kernel/mm/mem.o kernel/mm/buddy.o

# ===== Targets =====
.PHONY: all clean run runbin dump

all: k.elf k.bin

k.elf: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

k.bin: k.elf
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(ASFLAGS) -c $< -o $@

# Run using ELF as kernel (some QEMU builds accept this)
run: k.elf
	$(QEMU) -machine virt,gic-version=3 -cpu cortex-a53 -nographic \
		-serial mon:stdio -kernel k.elf -smp 1

# Fallback: load raw binary at our link address (matches linker.ld)
runbin: k.bin
	$(QEMU) -machine virt -cpu cortex-a53 -nographic \
		-device loader,file=k.bin,addr=0x40200000 -serial mon:stdio

dump: k.elf
	$(OBJDUMP) -d k.elf | less

clean:
	rm -f $(OBJS) k.elf k.bin

