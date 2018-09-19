# Opcodes

This section is incomplete. What follows is dump of my current ideas.

## Register stores:

Most of these have structure of `X OP Y`, which sets new value of `X` register to `OP(X, Y)`.
There are 4 registers, so they take 4x4 = 16 opcodes:
- ADC (add with carry)
- ADD (add without carry) - can be easily emulated, so may be dropped
- SBC (subtract with carry)
- SUB (subtract without carry) - can be easily emulated, so may be dropped
- MOV
- XOR
- OR
- AND
- CMP (same as SUB, but doesn't store)
- BIT (same as AND, but doesn't store)

(total: 16x10 = 160)

Non-two-register:
- LDI (load immediate)
- LOAD [H:L]/[0:imm]/[imm:L]/[0:L]
- LOAD SFRn (todo: what SFRs? I/O ports etc.)
- SHR/SAR/RCR/ROR

(total: 4x(~13) = ~52)

## Jumps

Jumps can have destination of `H:L` or `PCH:imm` for local jumps. There will be eight condition
codes, the same as MSP430's.

(total: 2x8 = 16)

## Flags

Set/clear C/N/Z/V.
(maybe FLAGS <-> reg ? Or maybe treat FLAGS as SFR?)

(total: 2x4 = 8)

## Stores

STR [H:L]/[0:imm]/[imm:L]/[0:L], A/B/H/L
STR SFRn, A/B/H/L

(total: 4x4 + 4x(~4) = ~32)

## Conclusion

As for what is written now, there are 268 possible opcdes, which is too much given we have only
256 opcodes available. We may however map SFRs in RAM, which would free us (and simplify ISA,
actually) roughly 32 opcodes (LOAD/STR SFRn).

Some of the opcodes may seem redundant (like `AND A, A`), but they can be treated as elaborate NOPs.
For ismplicity, we do not special case them.
