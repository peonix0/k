# Linker.ld

Here’s what I was wondering:

1. **Why `ALIGN(16)` and not `ALIGN(8)` or `ALIGN(32)`?**
2. **What’s up with `_bss_start` and the other markers?**

### **Why `ALIGN(16)`?**

* **16-byte alignment** is optimal for **AArch64**.
* **8-byte** is too small, and **32-byte** is overkill unless you're working with **SIMD** or special cases.
* **16 bytes** is a good balance for both performance and alignment.

### **What’s with `_bss_start` and others?**

* These markers like `__bss_start` and `__stack_bottom` are used to **track memory regions**.
* **`__bss_start` and `__bss_end`** help **clear uninitialized variables** in `.bss` by zeroing them out.
* **`_stack_bottom`** and **`_stack_top`** mark the **stack region** and help **set the stack pointer** (`sp`) during boot.

### **ChatGPT Recap**:

* **`KEEP`** ensures early code (like `_start`) isn’t dropped by the linker.
* **`ALIGN(16)`** is the best for **AArch64** performance.
* **`__bss_start` and `__bss_end`** track the **.bss section** for zeroing out uninitialized variables.
* **Stack setup** uses `_stack_bottom` and `_stack_top` to initialize the stack.



