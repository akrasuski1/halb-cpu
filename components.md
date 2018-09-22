# Components

There are a couple of components repeatedly used in this CPU, such as registers and multiplexers. They can
have unified design to simplify production.

## Registers

This schematic found [here](http://www.play-hookey.com/digital/alt_flip_flops/d_nand_flip-flop.html) looks
good on the first sight.

![D latch](http://www.play-hookey.com/digital/alt_flip_flops/images/denandff100.png)

It has a total of 6 NANDs, all of which are 2-input except for one 3-input. In TTL, that gives us
6 resistors and 19 transistors.

There is a pretty good criticism of this style of registers [here](http://www.megaprocessor.com/GBU_flip_flops.html).
In short, they are prone to data race - say we want to make a T flip flop from D by connecting negative
output to the input. Then, when output switches, input immediately changes too, possibly violating
hold times constraints. I think this issue persists even in proposed master-slave flip-flop
though (perhaps to a lesser degree). I guess I'll simulate both styles and then check the winner
in real life.

## Multiplexers

Since we represent high signal as pullup or lack of strong low signal, we can use simple connection
as wired-AND gate.

![mux](images/mux.png)

The output of the 2-bit decoder on the schematic acts as an enable signal on each of the inputs.
It would be better to design a "8-bit inverted buffer", i.e. 8 NANDs with one of the
inputs joined together as enables signal; and a 8-bit inverter on the right.

Inverted buffer would have 8 2-input NANDs, i.e. 16 resistors and 24 transistors;
8-bit inverter - 8 1-input NANDs, i.e. 8 resistors and 16 transistors.