# MPC500 bench card — 808 Lab (print)

**Version:** 1.0 · **2026-11-03**  
**Rig:** MPC500 only while producing · PC only for kit export (optional)  
**Pad law:** [SOP-012](../sops/SOP-012-spark-le-elite-system-charter.md) · **Roles 1–8 identical every session**

Related: [MPC500-ONLY-MUSIC-IDEAS.md](MPC500-ONLY-MUSIC-IDEAS.md) · [MPC500-PC-ONLY.md](MPC500-PC-ONLY.md) · [sprint-kits/](sprint-kits/)

---

## Pad map (every kit)

| Pad | Role | Assign from manifest `spark_le.pads` |
|-----|------|--------------------------------------|
| 1 | Sub / 808 | `OneShots/01_*` |
| 2 | Knock | `OneShots/02_*` |
| 3 | Snare body | `OneShots/03_*` |
| 4 | Clap / snap | `OneShots/04_*` |
| 5 | Closed hat | `OneShots/05_*` |
| 6 | Open hat | `OneShots/06_*` |
| 7 | Rim / perc | `OneShots/07_*` |
| 8 | FX | `OneShots/08_*` |

**BPM:** read `bpm` in each folder’s `spark-le-manifest.json`.

---

## Sprint packs (load order)

| # | Folder | Preset | BPM | Batch |
|---|--------|--------|-----|-------|
| 1 | `session-1-gangsta-trap/` | `lane-trap-or-die` | **140** | `14031103` |
| 2 | `session-2-dj-screw/` | `lane-dj-screw` | **72** | `72031103` |
| 3 | `session-3-smooth-jazz-rap/` | `lane-smooth-jazz-rap` | **86** | `86031103` |
| 4 | `session-4-trunk-sermon/` | `lane-fat-pat` | **84** | `84031103` |
| 5 | `session-5-stretch-uk-drill/` | `lane-uk-drill` | **144** | `14431103` |

Regenerate on PC (optional): `cd disklordz/website && npm run export:sprint-1-5`

---

## Project: “808 Lab reel” (sessions 1 → 2 → 3)

**Goal:** Three ~30s mixes **or** one chained song-mode set (A→B→C). **PC off** while recording.

### One-time setup per kit

1. Copy `OneShots/*.wav` into MPC sample memory.
2. Program **A, pads 1–8** per table above.
3. Set sequence BPM from manifest.

Optional: load `Loops/Tops_Kickless_2bar.wav` on a spare pad for chop reference.

---

### Sequence A — gangsta trap @ 140

**DNA:** `metro-roll-bar4` · **Listen:** knock vs sub bleed; optional hat roll bar 2 (pad 5 only).

| Bars | Programming |
|------|-------------|
| 1–4 | Pad **1** on 1; pad **2** on 2 & 4; pad **5** sparse 16ths |
| 5–8 | Add **5–6**; pad **8** max once per 4 bars |
| Mix | Pad **2** slightly **louder** than pad **1** |

**Export:** ~30s (8–16 bars), title e.g. *Pressure Lane*.

---

### Sequence B — screw @ 72

**Switch kit** to session 2 one-shots (same pad roles).

**Listen:** dull hats, long sub tail, snare **behind** the grid.

| Focus | Programming |
|-------|-------------|
| Pad 1 | Fewer hits; let decay ring |
| Pads 5–6 | Fewer, softer (“dull”) |
| Feel | Same grid idea as A, slower BPM |

**Export:** ~30s, title e.g. *Chopped Conscience*.

---

### Sequence C — smooth jazz rap @ 86

**DNA:** `smooth-jazz-8ths` · **Listen:** rim vs hat clutter; **no trap 32nd rolls**.

| Focus | Programming |
|-------|-------------|
| Pads 5–6 | **8th notes only** |
| Pad 7 | Rim on 2 & 4 |
| Pad 8 | Once per **8** bars (not 4) |
| Mix | Pad **1** **under** rim/hats (quieter than session 1) |

**Export:** ~30s, title e.g. *Smoky Cruise*.

---

### Finish (pick one)

| Mode | Steps |
|------|--------|
| **Three mixes** | Record A → B → C as separate ~30s exports (easiest for reviews). |
| **One suite** | Song mode / setlist: A @ 140 → B @ 72 → C @ 86 in one take (*MPC500 Scripture*). |

Score in [reviews/](reviews/) · Keeper when **Vibe/Groove ≥ 4** → [KEEPERS.md](KEEPERS.md).

---

## Sessions 4–5 (same pad habit)

| Seq | BPM | MPC focus |
|-----|-----|-----------|
| **D** Trunk Sermon | 84 | Fat snare pad 3, lazy hats, southern trunk |
| **E** Stretch Grid | 144 | Stretch grid; note keep/drop in sprint5 review |

---

## Creative toolbox (MPC only)

| # | Name | Do this |
|---|------|---------|
| 1 | Loop sermon | Kickless loop → chop to ~3 hits/bar in seq |
| 2 | Bank double | Bank A = sess 1, B = sess 2; mute tracks to flip vibe **same BPM** |
| 3 | Resample menace | Record 4 bars A → resample → pitch/filter → new pad 1 layer |
| 4 | 16-level hat | Pad 5 velocity = brush vs closed |
| 5 | 48-track prayer | Pass 1: pad 1 only; pass 2: + pad 2; … until full kit |
| 6 | One-bar air | Every 8 bars, bar 8: mute all except **one** rim hit |
| 7 | Trunk vs cruise | Same pads: trap kick 1+3 vs jazz kick **1 only** (two seqs) |
| 8 | Pad 8 as turn | Phonk/gangsta: FX every 4 bars; jazz: every 8 bars |
| 9 | Knock-first mix | Sess 1: 2 > 1; sess 3: 1 under hats/rim |
| 10 | Hat mute breakdown | Bars 9–12: mute 5–6; keep 1–4 + 7 |
| 11 | Pressure → scripture | Live A @ 140, stop kit switch, B @ 72, **one recorder pass** |

### Cross-session blends

| Blend | MPC method |
|-------|------------|
| Hard under smoke | Sess **1** pads 2–4 + sess **3** pads 1, 5–7 |
| Screw tops / trap sub | Long sample from sess 1 pad 1 + sess 2 hats/snare seq |
| Half-time illusion | Seq @ 140 half hats vs seq @ 72 same pads — A/B by ear |

---

## Review tags (after listening)

Use in review markdown: `kick-bleed` · `hat-mud` · `pocket-pull` · `perc-clutter`

| Session | DNA / focus |
|---------|-------------|
| 1 | `metro-roll-bar4` |
| 2 | Screw trunk, pocket behind grid |
| 3 | `smooth-jazz-8ths`, no trap rolls |
| 4 | Southern trunk lazy |
| 5 | Stretch / catalog decision |

---

## Checklist (session day)

- [ ] Kits on MPC (1–5 or today’s subset)
- [ ] Pads 1–8 mapped per manifest
- [ ] BPM set per sequence
- [ ] ~30s export per session (or reel chain)
- [ ] Vibe / Groove + tags in matching `reviews/2026-11-03_*.md`
- [ ] Promote keepers to KEEPERS.md when ≥ 4

---

*DiskLordz 808 Lab — built for MPC-ready manifests.*
