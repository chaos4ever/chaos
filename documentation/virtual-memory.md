# Virtual memory overview
## Layout of the linear address-space for a task

Start                       | Length        | Description
----------------------------|---------------|-------------
`0x00001000`                | `0x00001000`  | Process page directory.
`0x00002000`                | `0x00001000`  | Exception TSS:es.
`0x00008000`                | `0x00002000`  | Temporary mapped data.
`0x0000A000`                | `0x00001000`  | Kernel TSS.
`0x0000B000`                | `0x00001000`  | GDT/IDT.
`0x00100000`                | Varying       | Kernel code and data.
`0x00300000`                | `0x00100000`  | Process code and data. Used by `process_create`.
`0x00400000`                | `0x01C00000`  | Process' low-level memory allocation structures.
`0x02000000`                | `0x00400000`  | Process' page tables. Mapped using a [self-referencing page directory entry](http://wiki.osdev.org/Page_Tables#Recursive_mapping).
`0x02400000`                | `0x00400000`  | Process data. Parameters and other info.
`0x02800000`                | `SIZE_GLOBAL` | Global data area. Hifi-Eslöf, ports, etc...
`0x02800000 + SIZE_GLOBAL`  | Varying       | Freely disposable by process (process data and code, process heap etc.)
`0xFC000000`                | `0x04000000`  | Stack (lowest page is PL0, used for kernel-mode calls.)

## Format of the global memory space

Offset        | Length        | Description
--------------|---------------|-------------
`0x00000000`  | `0x02800000`  | Hifi-Eslöf structures.
`0x02800000`  | The rest      | Kernel data.

## Notes

There is also the concept of shared memory, which is a window in a process' address space mapping the same physical pages as mapped in another process. Through this space, the processes can communicate. This is used when large data structures are to be transferred between processes, to reduce the need for excessive copying (which is bad from a performance perspective.) An ordinary mailbox is often used to synchronize the transfers.

(At the moment, there are no system calls to support shared memory yet.)
