# NTS-1 multi-bass oscillator collection

This repo is a **library of swap-in user oscillators** for the Korg NTS-1 (and
binary-compatible Minilogue XD / Prologue builds). The NTS-1 loads **one** custom
osc at a time — each unit is a complete `.prg` you flash or swap via the logue
toolchain.

## Design principles

1. **One osc, many roles** — each unit uses a primary macro (**Vibe** or style-specific
   controls) instead of shipping separate “bass only” and “lead only” binaries.
2. **Genre-first** — phonk 808, P-Funk / West Coast, Juno 80s R&B, Dilla hip-hop
   bass; more styles can be added under `src/oscillators/<name>/`.
3. **Host integration** — musical units use NTS-1 **cutoff** and **resonance** where
   a synth filter is part of the sound (not on the kick drum osc).
4. **Presets in `manifest.json`** — the `_agent.presets` array documents knob
   values (0–100, or 0–2 for LP Type) for recall and for Cursor agents.

## Catalog

| Folder | Display name | Role on NTS-1 | Typical range |
|--------|--------------|---------------|---------------|
| `tr808-kick` | Phonk 808 | Kick / sub shots | C1–D#1 |
| `west-coast-moog` | P-Funk WC | Funk, Moog, West Coast bass & lead | E1–C3 |
| `juno-rnb` | Juno R&B | 80s R&B / pop bass | E1–A2 |
| `dilla-bass` | Dilla Bass | Hip-hop / neo-soul loose bass | D1–G2 |

Supporting folders: `template`, `wavetable` (learning / starters).

## Build all bass units

```bash
export LOGUE_SDK=/path/to/logue-sdk
./tools/build-all-bass.sh nutekt-digital
```

Outputs appear in each unit’s build directory per the logue SDK (`.prg` / loader
artifacts — see SDK docs).

## Workflow on hardware

1. Build the unit you need for the **current track**.
2. Load onto NTS-1 with the Korg logue utility (same flow as any custom osc).
3. Save **panel** cutoff/res and type settings in the NTS-1 preset if the unit
   uses host filter params.
4. Swap osc between sets — keep a cheat sheet of `_agent.presets` from each
   `manifest.json`.

## Adding another bass osc

1. Copy `src/oscillators/template/` to `src/oscillators/<new-name>/`.
2. Implement hooks in `.cc`, tune `manifest.json` (`header` + `_agent`).
3. Add a row to this doc and to `tools/build-all-bass.sh`.
4. Prefer **multifunction** macros over splitting into multiple binaries unless
   CPU limits force a split.

## Preset reference

Full preset blocks live in each unit’s `manifest.json` under `_agent.presets`.
Parameter order matches `header.params` left to right.
