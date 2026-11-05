# Sample Recipe Book (VSTi one-shots → MPC)

Human guide for the **Recipe Author** agent. Machine-readable recipes are emitted by `boom.py` as `recipe.json` (100 slots by default).

## Purpose

Turn a **style brief** into **raw mono WAV one-shots** (no keygroups). You load them into **Akai MPC Sample** and map pads yourself.

## Three agents, one command

| Agent | Role | Output |
|-------|------|--------|
| **Recipe Author** | Expand style → slot definitions | `recipe.json` |
| **Preset Matcher** | Pick factory program per slot | `assignments.json` |
| **Render Executor** | Load VST3, MIDI, trim, normalize | `*.wav` + `manifest.json` |

Start here:

```bash
cmake --build build -j --target VstiOneShotRender

python3 tools/vsti-one-shot/boom.py \
  --style "YOUR STYLE HERE" \
  --vst3 "/path/to/Plugin.vst3" \
  --out ~/Exports/my_pack
```

## Slot anatomy

Each slot defines **intent**, not a plugin:

- **id** — stable filename stem (`dark_memphis_stabs_042.wav`)
- **kind** — `stab` | `key` | `bell` | `pad_hit` (rotates across 100 slots)
- **root_midi** — single pitch; MPC transposes
- **max_capture_sec** — safety cap before tail trim
- **tags** — matcher hints (style tokens + kind)

Defaults for all renders:

- 44.1 kHz, velocity **100**, **mono** sum
- Peak normalize **−1 dBFS**
- Trim below **−60 dBFS**

## Matcher policy (factory presets)

- Factory program indices only (no preset file required for v0).
- Default **`--match-mode hybrid`**: name heuristics + **1.5 s preview renders** scored by slot kind.
- **`names`**: fast, metadata only. **`preview`**: audio only.
- Reuse penalty spreads programs across 100 slots (avoids one preset for the whole pack).
- Rationale is stored in `assignments.json` for humans; **render ignores it**.

## Render executor rules (strict)

- Input: `job.json` only.
- **No** changing programs, slots, or musical choices.
- Fail if any slot lacks `program`, `vst3_path`, or `root_midi`.
- Technical output only: WAV + `manifest.json`.

## Style brief tips

Good briefs mention **genre**, **tone**, and **role**:

- “Dark Memphis melodic stabs and warm lo-fi keys”
- “Bright French touch filter chords as short plucks”
- “Cyber funk mono leads, short decay, no long reverb tails”

The Recipe Author rotates four kinds across 100 slots so packs are varied, not 100 identical stabs.

## Agent SOPs

- [Recipe Author](agents/recipe-author.md)
- [Preset Matcher](agents/preset-matcher.md)
- [Render Executor](agents/render-executor.md)

## Related

- [../ARCHITECTURE.md](../ARCHITECTURE.md)
- [../../../tools/ARCHITECTURE.md](../../../tools/ARCHITECTURE.md)
