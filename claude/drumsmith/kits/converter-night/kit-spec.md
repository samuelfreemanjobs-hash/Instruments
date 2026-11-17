# Kit spec — Converter Night (CNV)

**Intake:** `intake-answers.md` · **R2P:** `R2P.md` · **SKU:** CNV  
**Generated:** 2026-11-17 · DRUMSMITH W1 product design  
**Ship status:** **DRAFT** — `release_ready=false` until RELEASE_GATE HITL

## Promise

Original DiskLordz **Memphis converter phonk** drums: rate-dropped subs, 12-bit / rompler snap, cutting EDGE hats, cavern claps, cassette dust. Not a brand-machine ROM dump.

**Store wedge (3 promises only):** rate-drop subs · converter snap · tape-cutting hats.

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

## Hit manifest (88)

| # | Role | Count | Folder | Banks | Naming prefix |
|---|------|------:|--------|-------|---------------|
| 1 | Kick | 12 | `01_…/Kicks/` | BIT12, ANLG, ROMP, SIMM, 707 | `CNV_01_KICK_` |
| 2 | Snare | 12 | `01_…/Snares/` | ROMP, RX, CAVE, BIT12 | `CNV_01_SNR_` |
| 3 | Clap | 4 | `01_…/Snares/` (or Claps/) | CAVE, metallic | `CNV_01_CLAP_` |
| 4 | Hat closed | 10 | `01_…/Hi-Hats_Closed/` | EDGE, ANLG, 808-lofi | `CNV_01_CH_` |
| 5 | Hat open | 6 | `01_…/Hi-Hats_Open/` | EDGE, ANLG, ROMP | `CNV_01_OH_` |
| 6 | Perc / cowbell | 8 | `01_…/Percussion/` | Latin≤, RATE cowbell | `CNV_01_PERC_` / `CNV_01_COW_` |
| 7 | Crash / ride | 6 | `01_…/Crashes_Rides/` | ROMP, RX | `CNV_01_CYM_` |
| 8 | Pitched sub | 12 | `02_…/Pitched_Sub_Basses/` | RATE | `CNV_02_SUB_` |
| 9 | Aliased one-shot | 6 | `02_…/Aliased_One_Shots/` | BIT12, ROMP | `CNV_02_ALS_` |
| 10 | Rhythm loop | 4 | `03_…/Acoustic_Rhythm_Loops/` | — | `CNV_03_LOOP_` |
| 11 | Top loop | 4 | `03_…/Top_Percussion_Loops/` | — | `CNV_03_TOP_` |
| 12 | Texture | 4 | `04_…/` | synth noise | `CNV_04_TEX_` |
| | **Total** | **88** | | | |

Full filename: `CNV_<CODE>_<VOICE>_<DESC>_<KEY|NA>_<##>.wav`  
Example: `CNV_02_SUB_RateDrop_F_01.wav`

### Count check

12+12+4+10+6+8+6+12+6+4+4+4 = **88** ✓

Plan draft row totals summed to 98; trimmed snares (−2), claps (−2), perc (−2), aliased (−2), textures (−2) to honor the locked **88** SKU size while keeping RATE/EDGE/ROMP hero weight.

## Storefront tree

```text
CNV_Converter_Night/
  01_RAW_HARDWARE_ONESHOTS/
    Kicks/  Snares/  Hi-Hats_Closed/  Hi-Hats_Open/
    Percussion/  Crashes_Rides/
  02_BITCRUSHED_AND_PITCHED/
    Pitched_Sub_Basses/  Aliased_One_Shots/
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
