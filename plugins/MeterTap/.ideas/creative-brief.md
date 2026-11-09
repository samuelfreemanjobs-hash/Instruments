# MeterTap — creative brief (DPCS Phase 1 PRD)

**Track:** APC greenfield · **Category:** `fx.metering` · **Class:** effect  
**Phase:** 1 — behaviour only (no implementation references required in this doc)

## Elevator pitch

MeterTap is a **stereo in / stereo out** utility effect that **passes audio unchanged** (aside from user trim) while showing **per-channel peak and RMS** levels on a simple meter UI. It exists to dogfood DiskLordz VST Factory: one factory-lib analysis block, one GUI meter module, DPCS discipline, factory QA.

## User stories

1. As an engineer, I insert MeterTap on the master bus to confirm levels before bounce.
2. As a factory operator, I run factory CI on MeterTap and get exit 0 without subjective listening.
3. As a debugger, I see meters fall to a defined floor on silence and never display NaN after unplug/replug.

## Audio behaviour

### Topology

- **Inputs:** stereo bus (L/R), audio-rate.
- **Outputs:** stereo bus (L/R), audio-rate.
- **Processing order per channel:** optional linear gain → analysis tap (peak + RMS) → output equals gained input (no dynamics, no filtering).

### Gain (trim)

- Parameter `dl_input_trim_db` adjusts linear gain applied to **both** channels equally before analysis and output.
- Range −24 dB to +12 dB, default 0 dB.
- Smoothing: 20 ms one-pole on parameter changes (audible zipper unacceptable on automation).

### Analysis (peak)

- **Peak** per channel: maximum absolute sample magnitude within the current GUI refresh window, converted to dBFS (0 dBFS = full-scale sine at |sample| = 1.0).
- **Hold:** optional peak-hold 1.5 s unless user clears via parameter `dl_peak_hold` (see parameter spec).
- Peak meter **attack** (display): 5 ms equivalent (faster than RMS).
- Peak meter **release** (display): 300 ms downward ballistics (standard VU-ish release for peak bar).

### Analysis (RMS)

- **RMS** per channel: sqrt(mean(sample²)) over a sliding window of **300 ms** at current host sample rate.
- Convert to dBFS using same full-scale reference as peak.
- RMS display uses **attack 300 ms**, **release 300 ms** on the **display value** (not the underlying detector window).

### Silence and extremes

| Condition | Expected meter / output |
|-----------|-------------------------|
| All-zero input for ≥1 s | Peak and RMS display ≤ −96 dBFS (floor); audio output silent |
| Sine at −12 dBFS | RMS within ±0.5 dB of −12; peak within ±0.5 dB of sine peak |
| Digital full scale square | Peak 0 dBFS; output clipped only if trim pushes over 1.0 — **trim shall not exceed +12 dB** so 0 dBFS in + 0 trim = at most 0 dBFS out |
| NaN/Inf at input | Output replaces non-finite samples with 0; meters show floor; **no NaN** at output |

### Bypass

- When `dl_bypass` is true, audio path is bit-identical to input (no trim applied in bypass mode). Meters may freeze at last value or show input post-bypass tap — **chose:** analyse **pre-bypass** input so user can verify levels while bypassed; document in UI label “Meters: input”.

### Latency

- Zero additional latency beyond host block; no lookahead.

## UI behaviour (`gui.meter`)

- Two vertical bars: **L** and **R**.
- Each bar shows **peak** (brighter segment) and **RMS** (wider/dimmer segment) per design-system minimum.
- Scale: −60 dBFS to 0 dBFS visible; values below −60 shown at bottom tick.
- Optional numeric readout (−∞ capped at −96) per channel — may ship in Phase 3 GUI slice if timeboxed.
- No editor resize persistence required for dogfood V1.

## MIDI / sidechain

- None.

## Presets

- Factory default single preset “Init” only for V1 (trim 0, bypass off, hold on).

## Acceptance (Phase 4 — forward reference)

Commands must exit 0 when implementation exists:

```bash
tools/build-vst.sh MeterTap
python3 vst-testing-ops/run_business.py --profile factory --plugin MeterTap
```

**Behavioural checks (automatable where noted):**

1. Render 1 kHz sine −12 dBFS 2 s stereo WAV through offline harness or `factory-audio` fixture — RMS within ±0.5 dB [future].
2. Silence 2 s — peak/RMS ≤ −96 dBFS in analysis JSON [future `analyze_wav`].
3. pluginval pass on Windows when operator online [tier A].

## Out of scope (V1)

- Spectrum / FFT (`gui.spectrum` taxonomy listed but **not** in Phase 0 constitution).
- Mid/side mode, surround, loudness (LUFS/ITU).
- MIDI learn, custom skins beyond default theme stub.

## Dependencies (conceptual only)

- DSP: `analysis.peak_rms` (factory-lib)
- GUI: `gui.meter` (factory-lib)
- Not in scope for Phase 1: JUCE types, includes, or file paths beyond this list.

## Links

- Constitution: [../DPCS_PHASE0_CONSTITUTION.md](../DPCS_PHASE0_CONSTITUTION.md)
- Parameters: [../parameter-spec.md](../parameter-spec.md)
- DPCS index: [../DPCS_PHASE1_PRD.md](../DPCS_PHASE1_PRD.md)
