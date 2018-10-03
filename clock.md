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
Write Enable is AND-ed with inverted clock (i.e. write happens in second half of the cycle,
when address and data should be long since calculated).

The clock will be generated using astable oscillator, at frequency adjustable in some narrow
range (less than a factor of five or so) by potentiometer. There will be a long chain of
prescaler units, each dividing frequency by two. The actual number of prescalers used will be
decided by DIP switches. There should be also possibility of using external clock, or even
manual button pressing (filtered to avoid glitches) as clock.

The oscillator frequency should be something very large, 10MHz perhaps. The last stage of
prescalers should go down to maybe 0.5Hz, to see the updates in real time. This totals
in about 24 T flip-flops.

[This](http://ch00ftech.com/2012/07/10/transistor-clock-part-2-prescaler/)
design of prescaler seems to be commonly used, and is relatively low on components
(one alternative is using D-flip-flop, which has 6 NANDs):

![prescaler](images/prescaler.png?raw=true)
