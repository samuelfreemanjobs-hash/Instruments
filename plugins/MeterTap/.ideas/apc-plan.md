# MeterTap — APC plan

**Architecture:** [architecture.md](architecture.md) · **Next:** `/apc-design MeterTap`

## Complexity assessment (spec §8)

| Dimension | Score | Notes |
|-----------|-------|-------|
| Voices | 0 | FX only |
| Oscillators/voice | 0 | none |
| Filters | 0 | none (no HPF, no weighting) |
| Modulation | 0 | none |
| FX | 1 | one insert: trim + analysis tap |
| Surfaces | 1 | VST + one preset (`Init`); no MIDI map, no hardware port |
| Targets | 0 | VST3 only (+ Standalone wrapper) |
| **Total** | **2** | |

**Tier:** L0 Minimal

**Risk flags (§8.3):** none. No voices, no file I/O, no audio-thread logging, no logue target.

**Plugin-specific risks**

| Risk | Mitigation |
|------|------------|
| RMS running-sum float drift over long sessions | `double` bucket sums + full recompute once per 300 ms window |
| Non-lock-free `std::atomic<float>` on some toolchain | `static_assert(is_always_lock_free)` in `MeterBridge.h` |
| Denormals on silent tails | `juce::ScopedNoDenormals` in `processBlock` |
| Peak window depends on UI timer rate | Peak = max since last UI read (`exchange`), so no samples are missed at any timer rate |
| Ballistics tied to timer jitter | `MeterBallistics` takes measured `dt`; tests use fixed `dt` |

## Tier override

OVERRIDE(spec §8.2 L0 guidance "single file Processor; inline DSP"): the MeterTap constitution's non-negotiable 1 (component-first) requires the meter DSP and widget to live in `vst/factory-lib/`. MeterTap exists to prove that library path. So it uses L1-style structure despite scoring L0:

- `params.json` (synced from parameter-spec v1)
- `vst/factory-lib/analysis/{PeakRms,MeterBridge,MeterBallistics}.h` + `vst/factory-lib/gui/LevelMeter.h`
- JUCE-free unit tests for the analysis headers (architecture §7, V1–V6)

## UI framework

| Field | Value |
|-------|-------|
| `ui_framework` | `juce-native` |
| Decision | JUCE Components + `juce::Timer` (30 Hz) |
| Rationale | Two bars, one button, optional knob. The template builds with `JUCE_WEB_BROWSER=0`. A WebView adds a browser runtime, a JS bridge and pluginval risk for no visual gain. `gui.meter` is specified as a JUCE stub (`vst/factory-lib/gui/LevelMeter.h`), so later SKUs reuse it directly. |
| Implementation strategy | Factory-lib widget + thin editor. Editor owns layout and attachments only. |

## Phase order from here

| Step | Command / artifact | Gate |
|------|--------------------|------|
| Design | `/apc-design MeterTap` → `.ideas/design-spec.md` | `tools/apc-gate.sh MeterTap design` |
| Factory approval | approve `approve_architecture` in VST Factory (Slack `#disklordz-factory-dept`) | DPCS Phase 2 entry |
| Contract (DPCS 2) | headers in architecture §9 allowlist; cmake compile only | compile exit 0 |
| Implement (DPCS 3) | one `.cpp` / factory-lib block per run | per-run allowlist |
| Verify (DPCS 4) | architecture §7 V1–V8 | all exit 0 |
| Ship | `/apc-ship MeterTap` → `release/MeterTap/` | `tools/apc-gate.sh MeterTap ship` |

Open issues A and B are resolved (operator, Plan). C–E in architecture §8 keep their defaults unless changed before the Contract phase.
