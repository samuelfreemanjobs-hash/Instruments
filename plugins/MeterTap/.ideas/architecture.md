# MeterTap — architecture (APC Plan)

**Phase:** APC Plan · DPCS between Phase 1 (PRD) and Phase 2 (Contract) 
**Inputs:** [creative-brief.md](creative-brief.md) · [parameter-spec.md](parameter-spec.md) · [../DPCS_PHASE0_CONSTITUTION.md](../DPCS_PHASE0_CONSTITUTION.md) 
**Normative:** [docs/dsp-architecture-specification.md](../../../docs/dsp-architecture-specification.md) · [docs/DPCS.md](../../../docs/DPCS.md)

No code in this document. Type and file names below are the Phase 2 contract targets, not implementations.

## 1. Core components

| Component (spec §3.1) | Lives in | Role |
|-----------------------|----------|------|
| **Context** | `vst/MeterTap/Source/PluginProcessor` | `sampleRate`, `maxBlockSize`, stereo layout; set in `prepareToPlay` |
| **ParameterBank** | APVTS in `PluginProcessor` | 4 params from parameter-spec v1; `dl_input_trim_db` smoothed |
| **Gain stage** | `shared/dsp/parameter_smoothing.h` (`DspParamSmooth`) | Existing one-pole smoother, 20 ms; no new DSP |
| **Sanitizer** | inline in `processBlock` | Non-finite input samples → 0 before anything else |
| **Peak/RMS detector** (`analysis.peak_rms`) | `vst/factory-lib/analysis/PeakRms.h` (new) | Audio-thread detector: block peak + sliding 300 ms RMS per channel; JUCE-free |
| **Meter bridge** | `vst/factory-lib/analysis/MeterBridge.h` (new) | Lock-free audio→UI handoff: per-channel atomics |
| **Meter ballistics** | `vst/factory-lib/analysis/MeterBallistics.h` (new) | UI-side display laws (attack/release/hold/floor) driven by explicit `dt`; JUCE-free, unit-testable |
| **Level meter widget** (`gui.meter`) | `vst/factory-lib/gui/LevelMeter.h` (new) | JUCE `Component`: one bar with peak + RMS segments |
| **Editor** | `vst/MeterTap/Source/PluginEditor` | Two `LevelMeter`s (L/R), bypass button, optional trim knob, "Meters: input" label |

Component-first (constitution non-negotiable 1): detector, bridge, ballistics and the widget live in `vst/factory-lib/`, never in `vst/MeterTap/Source/`. These files don't exist yet; `factory-lib` is currently README-only. Their contracts are part of Phase 2.

## 2. Program chain (audio thread, per `processBlock`)

Effect order per spec §4.3, reduced to what the brief needs: no dry/wet, no core algorithm.

```text
for each block (numSamples may be 0..maxBlockSize, any size):
  1. ScopedNoDenormals
  2. Sanitize       — per sample: !isfinite(x) → 0          (both channels)
  3. Read params    — bypass (bool), peakHold (bool, UI-only), trimDb target
  4. Branch on bypass:
     bypass = true   → analysis tap reads the sanitized INPUT; output = input (no trim)
     bypass = false  → per sample: g = smooth(10^(trimDb/20)); y = x * g
                       analysis tap reads y; output = y
  5. Analysis tap   — PeakRms.process(channelPtrs, numSamples)
  6. Publish        — MeterBridge.publish(peakMaxSinceLastRead, rmsNow) per channel
```

Decisions:

- **Gain is computed in linear domain then smoothed.** Target `10^(dB/20)` is computed once per block from the APVTS value. The smoother runs per sample on linear gain (20 ms, `DspParamSmooth::coeffFromMs`). Smoothing in dB would need a `pow` per sample. Linear-domain smoothing is fine for ±24 dB trim moves.
- **Bypass is the plugin's own parameter.** `getBypassParameter()` returns `dl_bypass`, so host bypass and the plugin button are the same control. While bypassed the smoother keeps tracking its target, so un-bypass does not jump from a stale gain.
- **No clipping, no limiting.** Float path, matching "no dynamics" in the brief. See Open issue A.
- **Latency = 0, tail = 0.** `getLatencySamples()` stays 0 and `getTailLengthSeconds()` returns 0.

### 2.1 Peak/RMS detector (`analysis.peak_rms`)

- **Peak:** max |x| over the block, folded into a per-channel "max since last UI read" accumulator (see bridge). This gives "peak within the current GUI refresh window" exactly as specified.
- **RMS:** sliding 300 ms window, implemented as a **bucketed running sum**:
  - 300 fixed buckets of 1 ms; bucket length = `round(sampleRate / 1000)` samples, set in `prepare`.
  - Each bucket stores Σx² (accumulated in `double`). Window sum = Σ buckets (`double`).
  - When a bucket completes: subtract the oldest bucket, add the new one, advance the index.
  - Every 300 buckets (once per window), the window sum is recomputed from scratch, which caps floating-point drift.
  - `rms = sqrt(windowSum / (300 * bucketLen))`. Output resolution is 1 ms, which is far finer than a 30 Hz UI.
- **Storage:** fixed `std::array` sized at compile time (2 ch × 300 buckets). It is independent of sample rate, so there is **no allocation, even in `prepareToPlay`**.
- **Reset:** `prepare()` and `reset()` zero all buckets. The first 300 ms after reset reads low, because the window is still filling, which is correct "sliding window" behaviour.

### 2.2 Meter bridge (audio → UI)

Per channel, two `std::atomic<float>`:

| Atomic | Writer (audio) | Reader (UI timer) |
|--------|----------------|-------------------|
| `peakSinceRead` | CAS-max with the block peak | `exchange(0)` = read and reset (defines the refresh window) |
| `rmsLatest` | `store(rms, relaxed)` | `load(relaxed)` |

- Requires `std::atomic<float>::is_always_lock_free` (static_assert in the contract header).
- One CAS loop per channel per block, not per sample.

## 3. Surface chain (message thread)

| Surface | Binding |
|---------|---------|
| APVTS | `dl_input_trim_db` float −24…+12 default 0 · `dl_bypass` bool false · `dl_peak_hold` bool true · `dl_meter_floor_db` float −96…−48 default −60, **not automatable** |
| Editor timer | `juce::Timer` at **30 Hz** pulls the bridge, advances `MeterBallistics` with measured `dt`, repaints both `LevelMeter`s |
| Bypass button | `ButtonAttachment` → `dl_bypass` |
| Trim knob (optional V1) | `SliderAttachment` → `dl_input_trim_db` |
| Label | "Meters: input" (brief §Bypass) |
| State | APVTS XML in `getStateInformation` / `setStateInformation`; parameter-spec v1 |
| MIDI | none (`NEEDS_MIDI_INPUT FALSE`) |

### 3.1 Display ballistics (`MeterBallistics`, UI thread)

All laws take an explicit `dt` (seconds), so offline tests are deterministic and independent of timer jitter.

| Bar | Attack | Release | Hold |
|-----|--------|---------|------|
| Peak | 5 ms time-constant toward a higher value | 300 ms toward a lower value | when `dl_peak_hold` is on: hold the max for 1.5 s, then release at 300 ms law |
| RMS | 300 ms | 300 ms | none |

- Values are converted to dBFS with `20·log10(max(v, 1e-12))`, then floored at **−96 dBFS** (brief §Silence).
- The bar scale is −60…0 dBFS (`dl_meter_floor_db` = display bottom). Values below are drawn at the bottom tick. The optional numeric readout uses the −96 floor and shows "−∞" at the floor.

## 4. Global layer

| Item | Value |
|------|-------|
| Bus layout | Stereo in / stereo out only (`isBusesLayoutSupported` rejects everything else) |
| Sample rates | Any host rate; tested at 44.1, 48, 96, 192 kHz |
| `prepareToPlay` | store SR, set smoother coeff (20 ms), `PeakRms.prepare(SR)`, reset smoother to the current target (no ramp on load) |
| `releaseResources` | nothing to free (all fixed storage) |
| Programs | 1: `Init` (trim 0, bypass off, hold on) |
| Plugin codes | Manufacturer `Inst` (repo convention from template), plugin `DlMT` (unique in repo, checked) |
| Formats | VST3 + Standalone (template default) |
| `params.json` | `vst/MeterTap/params.json` generated from parameter-spec v1 at Implement |

## 5. Threading and realtime contract

- **Audio thread may:** read APVTS atomics, run the detector on fixed storage, write bridge atomics.
- **Audio thread must not:** allocate, lock, log, or touch `juce::Component` / any GUI header.
- `vst/factory-lib/analysis/*` includes no JUCE headers (only `<atomic>`, `<array>`, `<cmath>`), so it can compile and be unit-tested without JUCE.
- `vst/factory-lib/gui/LevelMeter.h` is JUCE GUI and may only be included by the Editor.

## 6. Build wiring (Implement phase, not now)

- Scaffold with `./tools/new-vst.sh MeterTap --type effect` (from `vst/template-effect`).
- Add `target_include_directories(MeterTap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../factory-lib)` next to the existing `shared/dsp` include.
- Set `PLUGIN_CODE DlMT` and `PRODUCT_NAME "MeterTap"`.
- **Windows builds:** keep `COPY_PLUGIN_AFTER_BUILD` off (it writes to the system VST3 folder).

## 7. Verification plan (DPCS Phase 4 targets)

| # | Check | How | Exit-0 criterion |
|---|-------|-----|------------------|
| V1 | Detector maths | JUCE-free C++ test on `PeakRms` + `MeterBallistics` (ctest) | −12 dBFS 1 kHz sine, 2 s @ 48 kHz → RMS = −15.01 dBFS ±0.5 (see Open issue B); peak −12 ±0.5 |
| V2 | Silence floor | same test | 2 s zeros → peak and RMS display ≤ −96 dBFS |
| V3 | NaN safety | same test + processor test | NaN/Inf in → output finite, zeros where input was non-finite |
| V4 | Bypass bit-identity | processor test | bypass on, trim +6 dB → output == input bitwise |
| V5 | Trim law | processor test | after 200 ms settle, trim −6 dB → output/input = 10^(−6/20) ±1e−4 |
| V6 | Ballistics | `MeterBallistics` with fixed `dt` | peak hold keeps value 1.5 s ±1 tick, then decays; RMS 300 ms time-constant |
| V7 | Build | `tools/build-vst.sh MeterTap` (Linux CI) | exit 0 |
| V8 | Factory QA | `python3 vst-testing-ops/run_business.py --profile factory --plugin MeterTap` | exit 0 |
| V9 | pluginval | Windows, operator online (tier A, not CI blocker) | pass |

## 8. Open issues for Design / operator

These are contradictions or gaps in the Phase 1 docs. Each has a chosen default so Design can proceed. Changing a default needs a parameter-spec or brief delta.

- **A. Over-0 dBFS output — RESOLVED (operator, Plan).** No clipping in the plugin (float path, host handles headroom). Meters report the true value (can exceed 0 dBFS), and the bar shows a red "over" cap when the peak is > 0 dBFS. Brief updated.
- **B. RMS reference — RESOLVED (operator, Plan).** Unweighted RMS: a −12 dBFS-peak sine reads −15.01 dBFS RMS. Brief silence/extremes table and acceptance check 1 updated.
- **C. Bypass clicks.** "Bit-identical" bypass means toggling bypass with trim ≠ 0 dB steps the gain instantly, which can click. **Default:** literal spec (instant switch). If clicks are unacceptable, add a short crossfade on the transition only, keeping steady-state bit-identity.
- **D. Parameter id drift.** The constitution mentions `dl_mix_level`; parameter-spec v1 (later, versioned) uses `dl_input_trim_db`. **Default:** parameter-spec v1 wins. The factory intake prompt in `disklordz/vst-factory/data/meter-tap-dogfood.json` still says `dl_mix_level`; update it in a separate factory change.
- **E. `dl_meter_floor_db`.** It's in the spec but "not user-facing in V1 UI". **Default:** keep it as a non-automatable APVTS parameter fixed at −60, with no widget.

## 9. Phase 2 (Contract) allowlist

Headers only, compile-checked, no DSP bodies:

- `vst/factory-lib/analysis/PeakRms.h`
- `vst/factory-lib/analysis/MeterBridge.h`
- `vst/factory-lib/analysis/MeterBallistics.h`
- `vst/factory-lib/gui/LevelMeter.h`
- `vst/MeterTap/Source/PluginProcessor.h`, `vst/MeterTap/Source/PluginEditor.h` (after `/apc-design` and scaffold)
