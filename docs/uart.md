# PL011 UART

The PL011 is a UART controller that is mapped into the memory address space, and communication is done by reading and writing to its registers. As I write about this, I can't help but realize that I don't need to configure much to get this working—most of the setup is usually done by U-Boot.

My plan here is to send some random text through the UART. To do this, we need the address for the data register. In my code, I've only used the most important ones, such as the data register, which is primarily used for sending and receiving data over the UART serial interface. Other registers, like the flag register, are used to check the status of UART transmission—whether the UART FIFO is full or empty, and if we can send or receive data.

One thing to note: the data register in the UART is only 8 bits wide, but we’re referring to it as uint32_t. This is done to follow ARM conventions, where registers are typically 32 bits wide and data access is often done in 4-byte chunks. So, while the data register itself is only 8 bits, we still access it as a 32-bit value for consistency.
