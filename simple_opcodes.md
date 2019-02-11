# Simpler opcodes

Opcodes as shown on original opcode page seem to require quite a bit of complexity
in decoding stage, as well as synchronous register stores and some extra complexity
for dealing with multi-byte instructions (immediate values). I think I found a way
to remove them altogether, without sacrificing too much code density, at a cost of some
opcode space.

## `MOV reg, imm`

There can be just one instruction type accepting immediates: `MOV reg, imm`. Normally this would
be one two-byte opcode: `[MOV|reg][imm]`. However, we can do this in the same two bytes, but
consisting of two separate instructions: `XXX imm4, 0`, `XXX imm4, 1`. Opcode name is not yet decided,
hence temporary `XXX`. `imm4` in the above is a nibble of immediate to be moved, and the other
argument can be one of 3 values: 0, 1, 2. Every other execution of this instruction (as controlled
by a single bit flag) will do a different thing. The first one will store both arguments to `B`
register (using 6 of its bits). The second one will do the actual store to the target register.
The immediate to be stored is just a combination of the value in `B` and current argument.
The target register in turn can be calculated from the `0, 1, 2` part. Two of these give
9 combinations, enough to encode 9 targets. This will require 48 of 256 opcodes.
