# MeterTap — parameter spec (DPCS Phase 1)

**APVTS-only contract.** No C++, no default literals in code until Phase 2 headers.

## Global

| Property | Value |
|----------|--------|
| Plugin ID (proposed) | `DlMT` |
| Manufacturer | DiskLordz (per repo CMake convention at scaffold) |
| Programs | 1 factory: `Init` |

## Parameters

| ID | Display name | Type | Range | Default | Unit | Automatable | Notes |
|----|--------------|------|-------|---------|------|-------------|-------|
| `dl_input_trim_db` | Input trim | float | −24 … +12 | 0 | dB | yes | Linear gain mapping: `gain = 10^(db/20)` |
| `dl_bypass` | Bypass | bool | false / true | false | — | yes | When true: passthrough without trim; meters read **input** |
| `dl_peak_hold` | Peak hold | bool | false / true | true | — | yes | When true: peak bar holds max for 1.5 s then decays per release law |
| `dl_meter_floor_db` | Meter floor | float | −96 … −48 | −60 | dB | no | Display clamp; not user-facing in V1 UI (fixed default) |

## Smoothing

| Parameter | Smoothing time |
|-----------|----------------|
| `dl_input_trim_db` | 20 ms |
| others | instantaneous (bool) or internal ballistics only |

## GUI binding (logical)

| Widget | Binds |
|--------|--------|
| L peak/RMS bar | analysis state channel 0 |
| R peak/RMS bar | analysis state channel 1 |
| (optional) Trim knob | `dl_input_trim_db` |
| Bypass button | `dl_bypass` |

## State persistence

- All parameters saved in DAW preset and project recall.
- Bypass state restored on load.

## MIDI CC

- None V1.

## Versioning

- `parameter-spec` version: **1**
- Next change requires Phase 1 PRD delta + operator ack before Phase 2 header sync.
