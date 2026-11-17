# Kit spec — Converter Night (CNV)

**Store title (locked):** **Converter Night** · **SKU:** CNV · see `PRODUCT.md`  
**Intake:** `intake-answers.md` · **R2P:** `R2P.md`  
**Generated:** 2026-11-17 · DRUMSMITH W1 product design  
**Ship status:** **DRAFT** — `release_ready=false` until RELEASE_GATE HITL

## Promise

Original DiskLordz **90s Memphis rap / phonk** drums — converter crawl, 12-bit grit, horror space, club half-time bounce. Rate-dropped subs, aliased snap, cutting EDGE hats, cavern claps, cassette dust. Not a brand-machine ROM dump and not a recreation of any artist tape.

**Store wedge (3 promises only):** rate-drop subs · converter snap · tape-cutting hats.

**Producer-archetype lanes (internal ear targets only — no artist names on storefront):** Converter Crawl · Bitcrush Bed · Horror Snap · Club Bounce — see `recipes/MEMPHIS_ARCHETYPES.md`.

## Grid

| Param | Value | Tag |
|-------|-------|-----|
| Master BPM | 94 | A |
| Half-time loop BPM | 134 | A |
| SR / bit / ch | 44.1 kHz · PCM16 · mono | S (verify_kit) |
| Samples/bar @ 94 | `loop_samples(1, 94, 44100)` = **112596** | C |
| Samples/bar @ 134 | `loop_samples(1, 134, 44100)` = **78985** | C |
| Peak ceiling | −0.1 dBFS sample peak | S |
| Key anchor | F minor for pitched subs/cowbell | A |

## Hit manifest (512 — drums @ 3× guide + full useful FX)

Drums stay at the **3×** storefront guide; FX expands to every useful Memphis/phonk family through the **512** factory ceiling. Refs stay analysis-only; all ship hits are original synthesis.

| # | Role | Count | Folder | Banks | Naming prefix |
|---|------|------:|--------|-------|---------------|
| 1 | Kick | 48 | `01_…/Kicks/` | BIT12, ANLG, ROMP, SIMM, 707 | `CNV_01_KICK_` |
| 2 | Snare | 54 | `01_…/Snares/` | ROMP, RX, CAVE, BIT12 | `CNV_01_SNR_` |
| 3 | Clap | 24 | `01_…/Snares/` (or Claps/) | CAVE, metallic | `CNV_01_CLAP_` |
| 4 | Hat closed | 42 | `01_…/Hi-Hats_Closed/` | EDGE, ANLG, 808-lofi | `CNV_01_CH_` |
| 5 | Hat open | 24 | `01_…/Hi-Hats_Open/` | EDGE, ANLG, ROMP | `CNV_01_OH_` |
| 6 | Perc / cowbell | 30 | `01_…/Percussion/` | Latin≤, RATE cowbell | `CNV_01_PERC_` / `CNV_01_COW_` |
| 7 | Crash / ride | 40 | `01_…/Crashes_Rides/` | ROMP, RX | `CNV_01_CYM_` |
| 8 | Pitched sub | 60 | `02_…/Pitched_Sub_Basses/` | RATE | `CNV_02_SUB_` |
| 9 | Aliased one-shot | 48 | `02_…/Aliased_One_Shots/` | BIT12, ROMP | `CNV_02_ALS_` |
| 10 | Reverse / sweep | 40 | `02_…/Reverse_And_Sweeps/` | FX family | `CNV_02_REV_` |
| 11 | Stab / impact | 40 | `02_…/Stabs_And_Impacts/` | FX family | `CNV_02_STAB_` |
| 12 | Rhythm loop | 18 | `03_…/Acoustic_Rhythm_Loops/` | true 2-bar | `CNV_03_LOOP_` |
| 13 | Top loop | 18 | `03_…/Top_Percussion_Loops/` | true 2-bar | `CNV_03_TOP_` |
| 14 | Texture | 26 | `04_…/` | hum + dust | `CNV_04_TEX_` |
| | **Total** | **512** | | | |

Native plan mix: 808×60 · KICK×48 · SNARE×54 · CLAP×24 · HAT×66 · COWBELL×15 · PERC×15 · FX×230.

Full filename: `CNV_<CODE>_<VOICE>_<DESC>_<KEY|NA>_<##>.wav`  
Example: `CNV_02_SUB_RateDrop_F_01.wav`

### Count check

48+54+24+42+24+30+40+60+48+40+40+18+18+26 = **512** ✓ (factory ceiling)

## Storefront tree

```text
CNV_Converter_Night/
  01_RAW_HARDWARE_ONESHOTS/
    Kicks/  Snares/  Hi-Hats_Closed/  Hi-Hats_Open/
    Percussion/  Crashes_Rides/
  02_BITCRUSHED_AND_PITCHED/
    Pitched_Sub_Basses/  Aliased_One_Shots/
    Reverse_And_Sweeps/  Stabs_And_Impacts/
  03_LOOP_BUILDERS_AND_GROOVES/
    Acoustic_Rhythm_Loops/  Top_Percussion_Loops/
  04_LOFI_TEXTURES_AND_NOISE/
    Converter_Hum_And_Preamp_Noise/  Vinyl_And_Dust_Layers/
  _DOCS/
    ReadMe_License.txt
    Hardware_Processing_Guide.md
```

Build/QA tree (internal):

```text
kits/converter-night/
  masters/          # after render
  manifest.json
  references/       # analysis-only WAVs (gitignored)
  recipes/
  docs/
```

## Device exports

| Device | Status |
|--------|--------|
| Generic WAV PCM16 | INTERNAL after render |
| FL Studio / MPC / Maschine+ / MV-1 / Lofi12 / SP-404 | **UNVERIFIED** |

## RELEASE_GATE checklist (commercial)

From `docs/RELEASE_GATE.md` — all required before ship:

- [ ] W1 inventory matches rendered manifest
- [ ] W2 originals rendered (synthesis — not reference dumps)
- [ ] W3 `verify_kit.py` technical PASS + diversity; optional ffmpeg noted honestly
- [ ] Pitch screens only on stable pitched material; INCONCLUSIVE not labeled tuned
- [ ] CALIPER read-only audit (`docs/CALIPER_CURSOR.md`)
- [ ] Human listening log → `approvals.listening` (name + date)
- [ ] Human release approval → `approvals.release`
- [ ] Provenance + license + store copy reviewed
- [ ] No trademarked machine product title
- [ ] `release_ready=true` only when above complete

**Technical PASS ≠ ship.**

## DSP handoff

- Genre: prefer **`memphis-tape`** profile; extend CNV recipes under `recipes/`
- Voices: native kinds 808/kick/snare/clap/hat/cowbell/perc/FX + RATE pitch-env / alias stages
- Scripts: `native_factory.py` / recipe factory; `trap_loop_architect.py` for loop MIDI stubs
- RE: `references/` + skills `drum-sample-reverse-engineering`, `audio-analysis`

## HITL open

- [ ] Price · [ ] Cover · [ ] Ear · [ ] Legal EULA final · [ ] DR-660/SP-1200 ref upload (optional)
