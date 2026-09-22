---
name: sfz-multisample-map
description: "Build SFZ multisample maps from WAV sets and internal InstrumentMapSpec. Use for romplers, HISE import, and cross-DAW delivery."
---

# SFZ multisample map

## Strategy (Phase 1)

1. Build **InstrumentMapSpec** (internal JSON): groups, zones, root keys, velocity layers, sample paths, tuning cents.
2. Validate: no overlapping zones without intent; every file exists; roots within 0–127.
3. Emit `.sfz` + relative `samples/` layout.
4. Document round-robin and loop points when samples are looped.

## SFZ opcode baseline

Use minimally: `sample`, `group`, `key`, `lokey`, `hikey`, `lovel`, `hivel`, `pitch_keycenter`, `volume`, `pan`, `loop_mode`, `loop_start`, `loop_end`, `ampeg_*`, `fil_type`, `cutoff` as needed.

## Repo ties

- JD ROM zones: [docs/ROM.md](../../../docs/ROM.md)
- HISE lane may import SFZ — hand off via [hise-antigravity-handoff](../hise-antigravity-handoff/SKILL.md)

## Tests

- Parser round-trip or golden SFZ snippet in `disklordz/sound-factory/tests/` (add in implementation WO).
