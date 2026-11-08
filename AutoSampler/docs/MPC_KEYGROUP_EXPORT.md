# MPC keygroup export

Auto Sampler writes **Akai MPC firmware 2.x** keygroup programs (XPM **v2** XML) for MPC Live / One / X / Key.

Format and keyzone math are aligned with **[`push` PR #8](https://github.com/samuelfreemanjobs-hash/push/pull/8)** (`mpc-keygroup-generator/generate.js`):

- **Keyzones:** split-the-difference midpoints — `low = floor((prevRoot+root)/2)+1`, `high = floor((root+nextRoot)/2)`, coverage **0–127** without gaps.
- **XML:** `<Instrument>`, `<LowNote>`/`<HighNote>`, `<VelStart>`/`<VelEnd>`, `<KeygroupNumKeygroups>`, `<SampleFile>` basename only (resolved next to `.xpm`).
- **Root notes:** standard MIDI **0–127** (not offset +1).

## Layout

```text
<ProgramName>/
  <ProgramName>.xpm
  note_036.wav
  note_039.wav
  ...
```

Loops: when sustain loop points exist, layers set `<Looping>1</Looping>` with `<LoopStart>`/`<LoopEnd>`.

## Import

Copy the program folder into MPC content or use MPC Software **Import**. Validate in MPC 2.10+ if you rely on pad maps (we omit optional pad-map XML).

## Alternative toolchain

For JSON-driven builds or up to **four velocity layers** per zone without recording, use the standalone generator from `push` PR #8 after exporting WAVs from Auto Sampler.
