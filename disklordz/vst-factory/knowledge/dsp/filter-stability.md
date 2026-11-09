# Filter stability

Biquad coefficients must remain stable across sample-rate changes. Clamp Q; verify pole radius &lt; 1 for step response tests.

**Acceptance:** `test_biquad_step` — no NaN/Inf after 10k blocks at 48 kHz and 96 kHz.
