# More about DTL

This is continuation of measurements of very simple logic gates (previously in context of comparison of DTL and
TTL).

## Non-ideal components

After getting very good results in LTSpice and even real circuits using simple Schottky inverters,
I proceeded to build more complex structures, such as D flip-flop. It worked quite oddly, depending
on precise values of resistors used and clock used. After a few hours of bashing my head trying
to figure out why
apparently correct circuit fails to latch in real life, I noticed, in LTSpice, that input signal
slew rates matter a lot. And counter-intuitively, it worked better with more gentle slopes!

Flip-flop has many components and interconnections, so analyzing it just by looking at voltages
and currents vs. time wasn't very helpful. So, I stripped away a lot of things to minimize the
problem to smallest sample behaving in odd way.

## Diode capacitance

The problem was diode capacitance. The following tiny circuit, without even transistor,
shows the problem:

![capacitance](images/schottky_capacitance.png)

Normally, we would expect the middle point to be almost grounded (through a diode), with its
actual voltage between about 0.2V and 0.7V, depending on which diode is more limiting.
In this case however, we can see that the voltage actually drops below -2V, which should not happen.
It was pretty hard to reliably get this result - in LTSpice I generally set the fall time
to 1ns and disregarded very brief glitches; in real life, the slew rate varied with clock
source I used and how the circuit was loaded, which made the problem hardly reproducible.

The culprit turns out to be Schottky diode relatively high parasitic capacitance - in this
case, roughly 100-200pF. This is two orders of magnitude larger than usual 1N4148 or similar,
and comparable (larger even) to transistor's capacitance. When input falls quickly enough,
the capacitance dominates diode-like behavior. In case of simple inverter chain, as I tested
the design before, this was even beneficial, as it made the given inverter switch before
its input fell below 0.65V - as soon as it started falling with any respectable speed.
In more complex circuits, like aforementioned flip-flops, this caused erratic behavior,
with internal gates glitching. For example, when input quickly falls from 5V to 4.5V for a
moment, then recovers, which is acceptable, the output instead of staying low, creates a short
high pulse during that quick fall moment. In combinational circuitry this could be okay as long
as it is quickly resolved, but in stateful circuits (flip-flops) even a short glitch
is enough for it to switch state.

The same circuit as above, with Schottky switched to 1N4148 and original 1N4148 switched to two
of those, works as expected at all rise times (checked as low as 1ns and noticed almost no capacitance
effects).

Diode capacitance shows also as related problem: that the more inputs the gate has, the slower it
switches, with transition time roughly proportional to fan-in:

![fan-in](images/schottky_fan_in.png)

I'm pretty sure the reason is the same, and in this case causes the slow resistor to be forced
to charge more than one diode's worth of parasitic capacitance. The time constant
should be: 4k7 ohm * 100pF = 470ns in case of single diode and 5 times that for 5 diodes; LTSpice
simulates both almmost an order of magnitude smaller. This may be linked to the fact that we operate
diodes at almost "charged" state, which has unknown to me effects on time constant. In any case,
Schottky's seem to be a much less desirable an option now.
