# Milestones 1–5 implementation status

| Milestone | Included | Still required before release |
|---|---|---|
| M1: VST3/Standalone core | JUCE CMake, voice synthesis, MIDI, UI, state | Windows compile and DAW smoke test |
| M2: Hybrid workstation | Seven DSP timbres, WAV map zones and transposition | Higher-quality antialiased synthesis, loops, sample persistence, richer brass/choir models |
| M3: Composition & FX | Chords, arpeggiator, reverb, delay, drive | Tempo sync, reliable note-off behavior, pattern editor, robust automation smoothing |
| M4: MPC export | WAV renderer and JSON keygroups | Native MPC program compatibility, metadata and velocity-layer batch export |
| M5: Product | Factory patch picker, product metadata, release build configuration | 128 professionally voiced presets, CI builds, pluginval, installers, licensing review, performance profiling |

## Commercial release gates

1. Compile with pinned JUCE 8 commit on Windows x64; fix all compiler warnings and errors.
2. Run standalone and VST3 under REAPER, FL Studio, and a second host.
3. Run pluginval strictness 5 or higher, inspect no allocations/locks in processBlock.
4. Fix sample-bank swaps, export concurrency, host transport and arpeggiator timing.
5. Validate MIDI channel handling, chord note-off, voice stealing, preset restore and DAW project reopen.
6. Implement bandlimited oscillators and oversampled saturation; benchmark 16-voice worst case.
7. Add original sound library, installer, privacy/license notices, documentation and release signing.
