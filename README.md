# K -- ARMv8-A Kernel

Implements MMU setup, UART, GICv3, generic timer, and exception handling.

## Dependencies

```bash
sudo apt install gcc-aarch64-linux-gnu qemu-system-arm \
                 make binutils-aarch64-linux-gnu
```

## Build

```bash
make                # produces k.elf and k.bin
make clean

# Optional
make DEBUG=1
make PLATFORM=rk3588
```

## Run on QEMU (virt)

```bash
make run
# qemu-system-aarch64 -machine virt,gic-version=3 -cpu cortex-a53 \
#   -nographic -serial mon:stdio -kernel k.elf -smp 1
```

## Run on RK3588 via U-Boot (raw binary)

**Prereqs**

* Build `k.bin` with `PLATFORM=rk3588` and correct UART base/GIC init for RK3588.
* Ensure DRAM is up (U-Boot SPL/TPL handles this).

```bash
make PLATFORM=rk3588
```

**From U-Boot shell (SD/eMMC FAT/ext4):**

```bash
setenv loadaddr 0x40200000      # must match kernel link address
load mmc 0:1 ${loadaddr} k.bin

# Jump to entry (AArch64)
go ${loadaddr}
```

## Source Layout (brief)

| Path                     | Purpose                                               |
| ------------------------ | ----------------------------------------------------- |
| `boot/head.S`            | Early entry, EL setup, branch to `kmain`              |
| `boot/vector_el1.S`      | EL1 exception vectors                                 |
| `kernel/kmain.c`         | Main entry                                            |
| `kernel/mmu.c`           | Page tables + MMU enable                              |
| `kernel/uart.c`          | UART driver (PL011 on QEMU; DW UART on RK if enabled) |
| `kernel/gic.c`           | GICv3 init                                            |
| `kernel/generic_timer.c` | Timer init                                            |
| `kernel/exception.c`     | Exception handlers                                    |
| `kernel/debug.c`         | Logging helpers, implements stack backtrace           |
| `linker.ld`              | Memory map / entry address                            |
| `Makefile`               | Build + QEMU run targets                              |


