# {{PLUGIN_NAME}} — parameter spec (DPCS Phase 1)

**APVTS-only contract.** No C++ and no default literals in code until Phase 2 headers. 
**SOP:** [SOP-VF-01 Intake & Dream](../../../docs/disklordz/sops/SOP-VF-01-intake-dream.md) · mapping rules: [dsp-architecture-specification.md §7](../../../docs/dsp-architecture-specification.md)

## Global

| Property | Value |
|----------|-------|
| Plugin code (4 chars, unique in repo) | |
| Manufacturer | per repo CMake convention |
| Programs | 1 factory: `Init` |

## Parameters

IDs are lowercase snake_case with a product-family prefix (e.g. `dl_`). Never rename an ID after release (breaks saved projects).

| ID | Display name | Type | Range | Default | Unit | Automatable | Smoothing | Notes |
|----|--------------|------|-------|---------|------|-------------|-----------|-------|
| `dl_example_gain_db` | Gain | float | −24 … +12 | 0 | dB | yes | 20 ms | `gain = 10^(db/20)` |
| `dl_bypass` | Bypass | bool | false / true | false | — | yes | — | Host bypass parameter |

## Smoothing

| Parameter | Time | Domain |
|-----------|------|--------|
| | | linear / dB / Hz |

## GUI binding (logical)

| Widget (design-system component) | Binds |
|----------------------------------|-------|
| | |

## State persistence

- All parameters saved in DAW preset and project recall.

## MIDI CC

- None V1.

## Versioning

- `parameter-spec` version: **1**
- Any change requires a PRD delta + operator ack before Phase 2 header sync.
