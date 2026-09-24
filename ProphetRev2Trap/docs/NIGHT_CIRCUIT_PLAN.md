# Night Circuit — product plan

**Product name:** Night Circuit · **CMake target:** `ProphetRev2Trap`  
**North star:** [MILESTONE_1.md](MILESTONE_1.md) — **the playable instrument** (Rev2-*inspired* subtractive workflow, **not** Rev2 emulation; **synthetic** trap/vaporwave only).

## Milestone map

| Milestone | Meaning | Status |
|-----------|---------|--------|
| **M1** | Playable VST3 + dependable DSP + synthetic preset bank integrity | **In progress** (verification-first) |
| **M2** | Preset quality, GUI spec alignment, workflow (featured/favorites, performance) | After M1 gates |
| **M3** | Distribution (installers, signing, FL/MPC matrix complete) | After M2 |

## M1 — what’s in the tree (engineering baseline)

- 16-voice poly + **mono** + **legato glide**
- Dual osc (saw + pulse), **ladder LPF24**, analog-style sat/clip, **amp + filter ADSR**
- **Unison** 1–3, key tracking, circuit/filter drive
- **1,028** factory presets (21 foundations + variations) — **synthetic categories only**
- Category browser, featured filter, user preset save/load
- Targets: **FL Studio**, **MPC Software** (desktop)

M1 exit criteria: [MILESTONE_1.md](MILESTONE_1.md) + [../../docs/NIGHT_CIRCUIT_VERIFICATION.md](../../docs/NIGHT_CIRCUIT_VERIFICATION.md) Phases 1–2.

## M2 — queued (do not block M1)

| Item | Notes |
|------|--------|
| GUI vs [NIGHT_CIRCUIT_GUI_SPEC.md](../design/NIGHT_CIRCUIT_GUI_SPEC.md) | Panel layout, prev/next, Modified, favorites |
| Featured **50–100** | Sound-design subagent |
| Mod wheel / delay / chorus | One feature per QA host pass |
| **CLAP** | Optional SKU extension (root `clap_juce_extensions` pattern) |

## M3 / research (post-M1)

- PWM / third osc / Prophet-style **mod matrix** (identity stays Night Circuit, not clone)
- Built-in arp — low priority (DAW-first)
- MPE — only if SKU requires

## Category taxonomy

[Synthetic roles only — no acoustic presets.](PRESET_CATEGORIES.md)

## Related

- [ARCHITECTURE.md](../ARCHITECTURE.md) · [BUILD_BASELINE.md](BUILD_BASELINE.md)
- Agents: QA + sound-design subagent docs under `docs/NIGHT_CIRCUIT_*`
