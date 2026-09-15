# Vibe Mixing

**Vibe Mixing** is how VMPC2000XL turns language into mix moves. You describe the *feel* you want; the DAW interprets that brief and **creates a preset** — an ordered chain of internal VMPC plug-ins with parameters set to match your vibe.

It is not a replacement for your ears or external VSTs. It is a fast starting point: type → chain → refine with **UI** on each slot or by hand.

## How to use it

1. Open the plug-in rack at the bottom of the main window.
2. In the **Vibe Mixing** field, type a short brief (one line is enough).
3. Press **Enter** or click **Create vibe preset**.

The rack loads internal processors (typically **EQ-3 → Compressor → Limiter**, plus **Gain** or **Pan** when your words imply it) and applies the preset values.

### Example briefs

| You type | Tendency |
|----------|----------|
| `warm punchy club master` | Fuller lows, faster compression, limiter on |
| `lofi dusty tape vocals` | Rolled-off highs, heavier compression, mid focus |
| `bright airy open top` | High shelf lift, gentler attack |
| `gentle transparent glue` | Light compression, soft thresholds |
| `aggressive hype limit` | Heavy ratio, hot limiter ceiling |

Combine words freely — the engine scores keywords and merges them into one preset.

## What gets created

- A **named vibe** from your text (shown in the status line).
- An **explanation** of the chain (e.g. `EQ-3 → Compressor → Limiter`).
- **Slot assignments** starting at slot 1, in processing order.

External VSTs are untouched unless you load into the same slot index afterward.

## Architecture (today)

- `VibeMixEngine` — heuristic keyword interpreter (extensible for future ML/LLM backends).
- `PluginHostService::applyVibeMixFromBrief` — loads internal plug-ins and pushes parameter values.

Saved project recall for vibe briefs is planned; presets live in the rack until you change them.

## Related

- [INTERNAL_PLUGINS.md](INTERNAL_PLUGINS.md) — processor reference  
- [VST_HOSTING.md](VST_HOSTING.md) — external plug-ins on the same rack  
