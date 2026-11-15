# Songwriting Engine Implementation Plan

## Review

The supplied blueprint has the right dependency order: deterministic music data and MIDI correctness first, harmony/groove next, then editing UI and packaging. The original request mixes a full product roadmap with a first-milestone build directive; attempting to scaffold every directory up front would create an untestable shell. This implementation is therefore isolated under `songwriting-engine/` (moved from an audio-plugin-coder checkout) and delivered as a tested vertical slice before expanding.

The product is a standalone Windows MIDI-writing application, not a JUCE plugin. JUCE remains a possible later audio-engine dependency and does not belong in the first MIDI generator.

## Phases

1. **Foundation (implemented):** typed project model, beat-based events, supported chord parser, scales, legal voicing candidates, deterministic voice-leading selection, four coordinated tracks, style parameters, project JSON, Type 0/1 MIDI export and re-import validation, sample CLI, and tests.
2. **Harmony and groove:** chord syntax expansion, configurable scoring weights, passing chords and secondary dominants with explicit switches, style profile data files, richer shared rhythmic motifs, MIDI import into the project model.
3. **Desktop application:** Tauri 2 + React/TypeScript UI invoking the real Python engine; project settings, chord progression, arrangement summary, generate/export, and JSON save/load.
4. **Windows release:** PyInstaller sidecar, Tauri installer, clean-machine smoke tests, and documented release process.
5. **Later product work:** piano roll, chord pads, autonomous prompt-to-song pipeline, and a separately scoped JUCE audio engine.

## Verification and risks

- Exact engine checks: `python -m pytest` from `apps/songwriting-engine` and `python -m songwriting.cli generate --config configs/smoker_sketch.json --output-dir artifacts`.
- Python 3.12 and CMake are present. `mido` and `pytest` are installed for the selected Python.
- The earlier APC system check reports the WebView2 SDK missing. That blocks some JUCE WebView plugin builds, but not this Python MIDI engine. Tauri/Rust toolchains have not yet been verified.
- Type 0/1 structure, notes, tempo, meter, and note pairing are machine-validated; subjective feel and MPC compatibility require auditioning.
