# Parameter specification — {{PLUGIN_NAME}}

| ID | Name | Type | Range | Default | Unit | Smoothing | Notes |
|----|------|------|-------|---------|------|-----------|-------|
| gain | Gain | float | 0–1 | 0.7 | linear | 10ms | Output level |

## Presets

| Name | Values (id → value) |
|------|---------------------|
| Init | gain: 0.7 |

## Mapping notes

- Host automation: normalized per APVTS
- See `docs/dsp-architecture-specification.md` §7
