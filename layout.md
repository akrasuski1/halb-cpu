# Layout

Pretty much all of the CPU consists of NAND gates, so it is important to have a good repeatable
layout for them. Each of them needs VCC and GND, so these signals seem useful to have on bus
easily available everywhere. Between gates, there are also more signals, often passing through
quite long distance. For these reasons, I think using a stripboard with long traces is the best
choice here:

<img src="images/strip.jpg" width=300px/>

I have to check whether a cut between holes is feasible to make - it would allow to make more
compact layouts. Otherwise, I'd have to stay with drilling a whole hole in the trace. The latter
way has other advantage though: you can drill a whole stack of boards at the same time.
