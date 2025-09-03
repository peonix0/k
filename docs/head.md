# So what’s going on here??

Umm, this thing I can’t really get over (like her)! All I know is it’s some kind of optimization for memory access, relating to cache and all that jazz. So, at the top, we align the next sections to **128 bytes**. Then, about that vector thing—why is it aligned to **2048 bytes**? No idea yet, but I’ll figure it out later on...

Then the interesting bit was **SPSel** and **msr**. The **msr**/**mrs** instructions basically **set** or **get** system registers to and from general-purpose registers. These are used for controlling processor modes, saving/restoring status, and a lot of low-level stuff.

Here at the start, we’re **setting up registers to use SP\_EL1** (the stack pointer for **EL1**), and by the way, it also seems to configure the **Saved Program Status Register (SPSR)** to decide which mode to use. This part is still a bit above my head, but I’ll dive deeper into it later. We’ll probably touch on it again when we talk about **exception levels (EL)** more thoroughly.

Next, we’re setting up the stack. What’s crazy here is that all we did in the linker script was allocate **8KiB** of memory, and then we check it. The symbol **`_stack_top`** is a pseudo-instruction that’s essentially equivalent to these two instructions:

```asm
adrp x0, _stack_top
add x0, x0, :lo12:_stack_top
```

So, **`adrp`** loads the **upper 52 bits** of the address of **`_stack_top`** (essentially the **page address**), and **`add`** completes the address by adding the **lower 12 bits** (the **offset** within that page).

Once we assign this to the **sp** register, we’ve set up the **stack of 8KiB**. I'm not sure how the **stack\_bottom** limit gets checked yet, but it’s something we’ll figure out as we go.

After that, we move on to the **.bss section**, where we initialize all that allocated memory to **zero**. The **xzr** register is the zero register in ARM architecture, and **`[x1], #8`** means we’re incrementing **x1** by 8 bytes (**64 bits**), which effectively zeroes out the memory 8 bytes at a time.

Then there’s this **b.hs** instruction. It’s a **conditional branch** based on **unsigned comparisons** (whether **high** or **equal**). It’s not exactly clear to me why we didn’t use something like **b.lt** or **b.gt**—it might be tied to how the **flags** are set after comparisons (e.g., for unsigned integer comparisons).

By the way, the **`f`** and **`b`** suffixes on branch instructions indicate whether the branch should go **forward (f)** or **backward (b)**. Nothing too crazy, just standard ARM conditional branching.

