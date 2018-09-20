# General

## Architecture

RISC-style is the only reasonable way for homemade CPU, since anything more complicated would cause
the project to grow much too large to be made by hand. As for Harvard vs von Neumann - something akin
to AVR or MSP430 will be used, that is two distinct memories for program data ("Flash") and second for RAM. 
These will live in the same addressing space though (like in MSP430), with high bit distinguishing the two,
and with some instructions only able to operate on specific one of these (like in AVR). There will be
some small space (perhaps 16 addresses) at the lowest addresses, mapped to I/O and SFRs.

## Bus width

8-bit adressing is too little, since it would restrict the program size to just 256 instructions
(or rather 128 if we take half of the space for RAM). The next nice power of two is 16, which is
what will be used.

Another issue is data bus width. This could again be 16 bits for consistency, but it would make
most of the circuitry unnecessarily large. To reduce number of components we will stay at 8, which
is still useful enough (as proven by AVR).

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
