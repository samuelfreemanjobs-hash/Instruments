# BLACK BOX — Sonic DNA & drum voice inventory

**Goal:** Four distinct machine personalities (plus Dirty Circuits), not the same hits with different FX.  
**Evidence:** Product plan (user) + Memphis research in `converter-night/docs/` · tags: `[assumed]` profile numbers until ear-locked.

## Shared engine axes

| Axis | What it changes |
|------|-----------------|
| Pitch envelope depth / decay | Kick punch vs hollow boom |
| Bit depth + hold rate | Papery grit vs digital snap |
| Noise color / BP center | Hats, snares, claps |
| Saturation curve + drive | Clean machine vs trunk dirt |
| HPF / LPF shelves | Hollow archives vs bright 2000s |
| Body oscillator mix | Sine / triangle / square blend |

---

## BB93 — Memphis Archives (Underground)

**Feel:** Sparse, haunted, tape-eaten. Less aggression, more space.

| Voice | Count | DNA notes |
|-------|------:|-----------|
| Hollow 808 | 28 | Long-ish sub, soft pitch spike, dark LPF ~9–11 kHz |
| Papery snare | 32 | Thin membrane 200–280 Hz, long noise wires, low clip |
| Dry clap | 20 | Short flam, little body |
| Eerie cowbell | 18 | Detuned dual-square, slow decay, muted |
| Tape hats CH/OH | 36 | Soft EDGE, heavy tape LPF, 12-bit @ ~22–26 kHz |
| Grimy perc | 20 | Rim / clave / scrap metal |
| Loops | 12 | Sparse 94 BPM crawl |
| **Locked total** | **360** (doubled) | Mid band 175–225 |

**Profile knobs:** `bit_depth=11–12`, `tape_lpf=9.5k`, `clip_db=2–3.5`, `pitch_st_kick=36–42`

---

## BB96 — The Original Machine (Signature) ★

**Feel:** Flagship hardware character — thick, metallic, crunchy, Southern punch.

| Voice | Count | DNA notes | Naming |
|-------|------:|-----------|--------|
| 808 recipes | 72 | Thick / Punch / Crawl / Dirty (18 each). Long body: `808_duration≈1.65s`, `amp_decay_808≈0.72`, beater `808_click≈0.55` | `BB96_808_{Thick,Punch,Crawl,Dirty}_*` |
| Kick recipes | 48 | Punch / Knock / Thump | `BB96_KICK_*` |
| Snare recipes | 80 | Aggro / Paper / Crack / Flam | `BB96_SNR_*` |
| Clap recipes | 48 | Dry / Layer / Gate | `BB96_CLAP_*` |
| Hat recipes | 96 | Crunch / Dust / Thin / Metal (CH+OH) | `BB96_CH_*` / `BB96_OH_*` |
| Cow / tom / perc | 112 | Metal/Mute/Clang · Low/Mid/High/Floor · Rim/Clave/Scrap/Tick | `BB96_COW_*` / `TOM_*` / `PERC_*` |
| 2-bar loops | 24 | All converter-night blueprints (22+) · seed-varied voices | `BB96_LOOP_*bpm_*_*` |
| **Total** | **480** | Doubled flagship + recipe diversification | |

**Profile knobs:** `bit_depth=12`, `lofi_sr=26040`, `tape_lpf=11–12.5k`, `clip_db=5.5`, `pitch_st_kick=48`, `voice.diversify=true`, `amp_decay_808=0.72`

---

## BB98 — Trunk Pressure (Bass)

**Feel:** Sub-forward, cracking tops, street bounce.

| Voice | Count | DNA notes |
|-------|------:|-----------|
| Heavy sub kick | 32 | Longer amp decay, lower root C1–E1 |
| Booming 808 | 36 | Max sustain room for trunk |
| Cracking snare | 36 | Bright transient +4 dB attack feel |
| Layered clap | 24 | Multi-flam stacked |
| Rimshot | 16 | Hard click |
| Hard perc | 20 | Conga/tom hybrid |
| Hats | 28 | Tighter than BB93, less tape |
| Loops | 10 | 134 half-time bounce |
| **Locked total** | **388** (doubled) | 175–225 |

**Profile knobs:** `root_bias=-2st`, `amp_decay_kick=0.22`, `clip_db=5–7`, `hpf_snare=120`

---

## BB00 — Southern Circuitry (Digital Transition)

**Feel:** Cleaner converters, punchy digital, tuned 808s, tight hats.

| Voice | Count | DNA notes |
|-------|------:|-----------|
| Punchy digital kick | 32 | Less pitch spike, cleaner sine |
| Snappy snare | 36 | Short noise, higher BP |
| Bright clap | 24 | Less tape, more presence 3–5 kHz |
| Tight hats | 40 | Short CH, controlled OH |
| Synced perc | 24 | Cleaner metallic |
| Tuned 808 bass | 40 | Keyed chromatic set |
| Loops | 12 | 95–100 / 140 feel |
| **Locked total** | **416** (doubled) | 200–250 |

**Profile knobs:** `bit_depth=14–16`, `lofi_sr=44100` (light or off), `tape_lpf=16k`, `clip_db=2–3.5`

---

## Dirty Circuits (bus)

Not a fifth “era machine” — a **corruption bus**: extra bitcrush, hard clip, alias, saturation applied to BB96 (and optionally other eras) for Clean→Dirty pairs + exclusives.

| Mode | Use |
|------|-----|
| CLEAN | Era profile as-is |
| DIRTY | +2–4 bit crush, +drive, optional downsample |
| RUINED | Extreme alias + soft-clip bus |

---

## Distinctness checklist

Before shipping any two eras, verify:

1. Blind A/B: kick from BB93 vs BB96 vs BB98 vs BB00 distinguishable in 2 seconds  
2. Hat grit / brightness ladder: 93 darkest → 00 brightest  
3. Sub weight ladder: 98 heaviest → 93 hollow → 00 tight  
4. No shared identical PCM across SKUs (diversity SHA check)
