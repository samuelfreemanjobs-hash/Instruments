# Denormal handling

Subnormal floats in IIR feedback paths can burn CPU. Use `juce::ScopedNoDenormals`, FTZ/DAZ flags, or DC offset in filter loops.

**Acceptance:** `test_denormal_sweep` — 60 s silence after impulse, CPU time bounded vs baseline.
