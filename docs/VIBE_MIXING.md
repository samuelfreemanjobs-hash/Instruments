# Vibe Mixing

**Vibe Mixing** turns language into mix moves. You describe the *feel* you want; Resonance interprets the brief and proposes an ordered chain of internal plug-ins with parameters set to match your vibe. You **preview** the chain (with rationales and conflict notes), pick a variant if you want, then **Apply**.

It is not a replacement for your ears or external VSTs. It is a fast starting point: type → preview → apply → refine with **UI** on each slot.

## How to use it

1. Open the plug-in rack at the bottom of the main window.
2. Choose a **target** (master, full mix, drums/vocals/synths group, or a specific channel insert).
3. Type a short brief (or tap a **chip** suggestion).
4. Click **Preview vibe** or press **Enter**.
5. In the preview dialog, review slots and rationales, try **Brighter** / **Darker** if needed, then **Apply vibe**.

After apply you can **Undo vibe**, **A/B compare** the last before/after mix state, **Copy JSON** (schema v1 recipe), or **Save vibe** into the project file.

### Example briefs

| You type | Tendency |
|----------|----------|
| `warm punchy club master` | Fuller lows, faster compression, limiter on |
| `lofi dusty tape vocals` | Rolled-off highs, tape sat, de-esser on vocals target |
| `bright airy open top` | High shelf lift, gentler attack |
| `wide spacious synths` | Stereo width on synth channel target |
| `gentle transparent glue` | Light compression, soft thresholds |

Combine words freely — keyword scoring, **archetypes**, and the selected **target profile** merge into one preset.

## Targets

| Target | Where it applies |
|--------|------------------|
| Master bus | 8-slot master rack |
| Full mix | Same as master (semantic “whole mix” profile) |
| Drums / Vocals / Synths | Channel inserts 1–3 (compact 2-slot chains) |
| Channel insert | Any mixer channel you pick |

## What gets created

- A **named vibe** from your text.
- An **explanation** and per-slot **rationales** in the preview.
- **Conflict hints** when words pull in opposite directions (e.g. bright vs lofi).
- Optional **brighter** and **darker** variants in the preview dialog.

Internal chain can include **HPF**, **EQ-3**, **De-Esser**, **Compressor**, **Tape**, **Width**, **Limiter**, **Gain**, and **Pan** depending on the brief.

## Persistence

- **Brief history** and **saved named vibes** are stored under `<Mixing>` in `.resonance` projects.
- Recipes can be exported as **JSON v1** (`VibeRecipeJson`) via **Copy JSON**.

## Undo / A/B

- **Undo vibe** restores the mix snapshot taken immediately before the last apply.
- **A/B compare** toggles between that pre-apply snapshot and the post-apply snapshot.

## Architecture

- `VibeMixEngine` — keywords, archetypes, target profiles, variants.
- `VibeTuning` / `VibeArchetypeLibrary` — tuning and curated blends.
- `PluginHostService::interpretVibeMix` / `applyVibeRecipe` — scoped apply to master or channel inserts.
- `VibeMixPreviewDialog` — trust layer before mutating the rack.
- `PluginSlotChain` — atomic runtime swap so the audio thread does not drop blocks during slot updates.

## Related

- [INTERNAL_PLUGINS.md](INTERNAL_PLUGINS.md) — processor reference  
- [VST_HOSTING.md](VST_HOSTING.md) — external plug-ins on the same rack  
