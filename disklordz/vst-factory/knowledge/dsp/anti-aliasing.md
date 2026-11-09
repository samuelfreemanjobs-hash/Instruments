# Anti-aliasing (factory note)

Hard-synced oscillators and waveshapers alias without band-limiting. Prefer polyBLEP/minBLEP for naive saw/square, or run nonlinear stages at 2×–4× OS.

**Acceptance:** sine-only path → THD+N below -60 dB at 1 kHz; saw without BLEP must document intentional crunch.

**See:** `vst/factory-lib/tests/PLAN.md`, shared/dsp helpers.
