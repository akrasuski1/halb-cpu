# Loading program

The program data is stored on Flash chip, which is connected through a set of pins (and thus detachable).
It would be nice to have a way to program the chip without using any exernal components, such as computer
or Arduino. So far I thought of two alternatives:

## Manual

This would consist of making a small board with several on-off switches for setting address and data,
and a button connected to Write input of the chip. Programmer would flip the switches slowly loading
the program. To help debugging, a display of address and data buses would be useful.

## Punch tape

A more hardcore way would be to make a punchcard reader. It would be a simple format, and actually it
would emulate much of what would happen in manual process:

```
W AAAA DD
0 0000 dd
1 0000 dd
0 0000 dd
0 0001 ee
1 0001 ee
0 0001 ee
0 0010 ff
1 0010 ff
0 0010 ff
```

1 represents a hole, and 0 - no hole. The tape would be passed through the reader, and the needed
waveforms would automatically be generated.

## Switchboard

Another way would be to allow other device than Flash to be connected as program memory.
It could be something like diode ROM, but it is extremely slow in programming (a bit per minute maybe?).
Instead, you could have a switchboard of a kind, or a board with tens of DIP 8-switches, correctly
multiplexed and labeled, to allow a small program to be input by flipping switches.
