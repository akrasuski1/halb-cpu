# HALB-CPU

This is a design document for simple, yet reasonably functional CPU.

# Design assumptions and decisions

The main idea is to make a CPU that is possible to create from scratch in home. By "from scratch" I mean
using nothing more than discrete transistors, diodes, resistors and whatever needed for connecting them.
There might be some exceptions, such as clock generation or I/O, but the core should follow these principles.

## TL;DR

ROM and RAM each take half of 16-bit address space with 8-bit data cells; RISC-style ISA
with 4 registers, two of which can act as pointer; all in TTL logic; each opcode takes two cycles.

## Table of contents

- [General issues](general.md)
- [Transistor technology (DTL vs TTL)](dtl-vs-ttl.md)
- [Opcodes](opcodes.md)
- [ALU design](alu.md)
- [Clock generation](clock.md)
- [Loading program](loading-program.md)
