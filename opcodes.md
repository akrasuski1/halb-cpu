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
- SHR/SAR/RCR/ROR (maybe leave rotates out, they are pretty useless)
- INC/DEC
- NOT (XOR 0xFF - useful on its own, and to make NEG using identity `-x == (x ^ 0xff) + 1`)

(total: 4x(~12) = ~48)

## Jumps

Jumps can have destination of `H:L` or `PCH:imm` for local jumps. There will be eight condition
codes, the same as MSP430's.

(total: 2x8 = 16)

## Flags

Set/clear C/N/Z/V.

(total: 2x4 = 8)

## Stores

STR [H:L]/[0:imm]/[imm:L]/[0:L], A/B/H/L

(total: 4x4 = 16)

## Conclusion

As for what is written now, there are 248 possible opcodes, which uses most of opcode space, but
leaves a bit for possible future expansion.

Some of the opcodes may seem redundant (like `AND A, A`), but they can be treated as elaborate NOPs.
For simplicity they are not special cased.
