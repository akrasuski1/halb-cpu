# HALB-CPU

This is design document for simple, yet reasonably functional CPU.

# Design assumptions and decisions

The main idea is to make the a CPU that is possible to create from scratch in home. By "from scratch" I mean
using nothing more than discrete transistors, diodes, resistors and whatever needed for connecting them.
There might be some exceptions, such as clock generation or I/O, but the core should follow these principles.

## TL;DR

ROM and RAM each take half of 16-bit address space with 8-bit data cells; RISC-style ISA
with 4 registers, two of which can act as pointer.

## Architecture

RISC-style is the only reasonable way for homemade CPU, since anything more complicated would cause
the project to grow much too large to be made by hand. As for Harvard vs von Neumann - something akin
to AVR will be used, that is two distinct memories for program data ("Flash") and second for RAM. These
will live in the same addressing space though, with high bit distinguishing between the two.

## Bus width

8-bit adressing is too little, since it would restrict the program size to just 256 instructions
(or rather 128 if we take half of the space for RAM). The next nice power of two is 16, which is
what will be used.

Another issue is data bus width. This could again be 16 bits for consistency, but it would make
most of the circuitry unnecessarily large. To reduce number of components we will stay at 8, which
is still useful enough.

## Registers

The most obvious register is Program Counter, or PC henceforth. Due to address size, it has to be 
15-bit.

As all RISCs, there will be general purpose registers too. Two is the minimum useful number of registers
to perform any calculation more advanced than pure accumulation. There is problem with memory writing
though - say we want to store some value, say 5, to address 1234. These are 3 bytes of information
in total: one byte for value to be stored and two for address. That leaves us with either having
at least one 16-bit register acting as a pointer, or having at least three registers. It is unclear
how standard 8-bit ALU operations would operate on 16-bit register and special-casing it would mean
complicating the circuitry. Thus, the other option is taken: at least three, and actually four
registers. An extra one is added both to round up to power of two, and to help in modifying the
address itself.

Two registers must be used to address whole memory. Using any pair could be allowed, but would
increase complexity, so instead two registers are designated as the only ones that can be used
as pointers: H and L. The other two are called generically, A and B. This is where name of the
CPU comes from: HALB are register names, and also German for "half", which fits quite well
given that we cut corners where possible.

## Opcodes

First of all, what should be the opcode size? Well, we pretty much have to have ability to load
constants to registers. Other than clumsy solutions like loading just 4 bits in one operation,
we are left with having to use more than 8 bits for at least some opcodes. Fractional bytes
are out of the question, so these loads will use two bytes. Most of the other operations don't
need so much though: an `ADD r1, r2` for example would have only 16 possibities. In that case,
we allow two formats of instructions: a single-byte instruction, and two-byte, where the
second is used only as immediate value and nothing else.

### ALU

What operations should ALU support? The core of the ALU will be adder-subtractor, so `ADD` and
`SUB` are definite candidates. For "free" we may add `ADC` and `SUBC` to allow multi-byte
operations - these may even be the only available, forcing user to reset carry flag manually
beforehand. There has to be `MOV` too, implemented as passthrough the ALU. Some bit operations
are welcome as well: most commonly, `XOR`, `AND`, `OR`. Perhaps unary `NOT` or `NEG` too,
but these can be emulated pretty easily. Multiplication is too hard, but can be implemented
in software using shift-and-add. We have to have shifts then! Left shift is actually the same as
`ADD r1, r1`, but right shift needs to be explicitly done in hardware. Rotating and/or rotating
through carry might be useful too, as well as arithmetic shift for signed division by two.
Many of these can be emulated, so it all depends on resources.

### Other operations

Definitely `LOAD` and `STORE`. That might be just enough actually, as far as memory is concerned.

Constant loads - `LDI`, two-byte instruction for loading constant to a specified register.

We need some flow control too. Calls and returns are too complex, so jumps have to suffice. The
former two can be, with some difficulty, emulated by carefully crafted assembly. As for
jumps, they aren't too easy either. The main problem is PC is 16-bit while we only have
8-bit immediate available. The only simple solution is to, again, use a pair of registers
as jump destination. This is less than perfect, as that means registers are often clobbered.
A partial mitigation would be to allow "short" jumps with the same "page" of memory, through
setting just the least significant byte of PC to immediate value. Another hybrid solution
is to allow jumping to address `A:imm`, that is loading register `A` to high byte of PC,
and immediate to lower; thus clobbering only one of valuable registers.

What remains is a number of miscellaneous yet useful instructions, such as setting/clearing
ALU flags.


# Technology

It is yet undecided whether DTL (Diode-Transistor Logic) or TTL (Transistor-Transistor Logic) will be used.
The former is simpler and uses less components, but TTL is much faster - in preliminary experiments,
there's roughly an order of magnitude of difference.
