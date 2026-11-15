# MpcVoyager — design spec

- Mono last-note MIDI; legato re-triggers glide from currentFreq.
- Per-sample loop: glide → osc morph ×3 + noise → ladder (mod cutoff) → tanh → amp env.
- Factory programs: 16 named G-funk/West Coast patches in `FactoryPrograms.h`; JSON bank stub for future 200-preset expansion.
- No editor; aarch64 via `build-vst-aarch64-linux.sh MpcVoyager`.