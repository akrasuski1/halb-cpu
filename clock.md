# Clock

The whole CPU will use a single master clock. Most instructions will take a single cycle,
except of those that have immediate value, which take two. 
To ensure correct start, some kind of power-on reset
is required to ignore first cycle or two.

During rising clock edge, every temporary signal will be latched to appropriate registers,
the rest of the clock cycle is used to propagate new values of registers to calculate
new temporary values. The only time that clock value matters other than its rising edge,
is when storing data to RAM. Since most of RAM chips are level-triggered, if we were to
output the Write Enable signal as soon as we decode the instruction, we would risk
not having address ready yet and thus writing in bogus location. For that reason,
Write Enable may be AND-ed with inverted clock (i.e. write happens in second half of the cycle,
when address and data should be long since calculated). This may however cause hold
time violation for that chip (we're releasing WE line at about the same time as invalidating
address and data buses). For that reason we may consider making 2-bit Gray code counter and
using one of the outputs as clock as always, and using the other output as RAM access
clock: 

```
With Gray code:

Q0   '''''''''|_________|'''''''''|_________|'''''''''|_________
Q1   ''''|_________|'''''''''|_________|'''''''''|_________|''''
rising edge        x                   x                   x    
WE   WWWWWWWWW           WWWWWWWWW           WWWWWWWWW          

Old way:

clk  ''''|_________|'''''''''|_________|'''''''''|_________|''''
rising edge        x                   x                   x    
WE        WWWWWWWWW           WWWWWWWWW           WWWWWWWWW     
```

This would allow
at least half clock period of both setup and hold. Disadvantage is that in manual clock mode
user will have to press button twice for proper cycle; and that clock will have to double
its speed in general. The Gray code generator will have to be very reliable too, even
at high speed, and have steep slopes.

The clock will be generated using astable oscillator, at frequency adjustable in some narrow
range (less than a factor of five or so) by potentiometer. There will be a long chain of
prescaler units, each dividing frequency by two. The actual number of prescalers used will be
decided by DIP switches or similar method. There should be also possibility of using external clock, or even
manual button pressing (filtered to avoid glitches) as clock. The latter two have potential
to have rather slow rise time - so the clock signal will be passed through 4 inverters,
similarly to Darlington pair, in order to discretize the clock as much as possible:

![clock](images/clock_inv.png?raw=true)

The measured rise time is about a nanosecond or two, which should be enough.
(update: actually, two inverters is enough too, with R_base = 100, R_pullup = 270).

The oscillator frequency should be something very large, 10MHz perhaps. The last stage of
prescalers should go down to maybe 0.5Hz, to see the updates in real time. This totals
in about 24 T flip-flops.

[This](http://ch00ftech.com/2012/07/10/transistor-clock-part-2-prescaler/)
design of prescaler seems to be commonly used, and is relatively low on components
(one alternative is using D-flip-flop, which has 6 NANDs):

![prescaler](images/clock_div.png?raw=true)

After checking it, it seems to be very sensitive on input clock frequency though - for some capacitor
and resistor values, it works at 1MHz, for others - 100kHz etc. Otherwise there are either
extra unwanted oscillations, or the output stays constant the whole time. In the end, I'll
probably stick with ordinary D flip-flops, regardless of their size, since they work more reliably.
