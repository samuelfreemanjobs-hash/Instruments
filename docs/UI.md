# JD Upgraded — UI

See [docs/ARCHITECTURE.md](ARCHITECTURE.md) for processor wiring.

## Layout

- **Programs** — prev/next, factory name, **Export** / **Import** (`.jdpreset` APVTS XML).
- **ROM browser** — pick tone A–D, wave index slider, prev/next, **waveform preview** (`WavePreviewComponent`).
- **FX row** — master, filter link/resonance, Group A/B (+ enables, chorus), coupling.
- **Tones** — level, wave, multisample, TVF, mute, palette category snap.
- **Envelopes** — global link or per-tone ADSR.

## Expression (MIDI)

- **Pitch wheel** — ±2 semitones on all active voices (applied in `applyPatchesFromParameters`).
- **Channel pressure / poly aftertouch** — opens filter cutoff when **Expression to Filter** &gt; 0.

## Preset files

`.jdpreset` is JUCE `ValueTree` XML with `currentProgram` and `format=JDUpgradedApvts`. Not interchangeable with Roland SysEx; use SysEx import for JD hardware patches.
