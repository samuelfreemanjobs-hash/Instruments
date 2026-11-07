# MPC keygroup cross-check

Auto Sampler’s C++ exporter implements the same **XPM v2** rules as the Node generator in [push PR #8](https://github.com/samuelfreemanjobs-hash/push/pull/8).

To cross-validate a captured kit manually:

1. Export MPC kit from Auto Sampler (or mega bank).
2. Clone the `push` generator and rebuild `.xpm` from the same WAVs.
3. Diff `LowNote`/`HighNote`/`VelStart`/`VelEnd` tags.

Velocity-layer programs with more than four layers must be split before MPC import.
