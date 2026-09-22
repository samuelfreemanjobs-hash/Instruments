# logue custom oscillator collection — roadmap

**Owner intent:** **NTS-1 mkII only** for new units (`.nts1mkiiunit`). v1.1 / Minilogue XD builds are **out of scope** unless a legacy port is explicitly revived.  
**Status:** Planning (2026-09-22, expanded). **25 oscillator products** + **2 custom FX units** + Phase 0 infra. Update as slugs ship.

**Scope split:** This repo’s agent tooling today is **osc-first** (`src/mkii/oscillators/`). **SP-1200** and **dream reverb** are **logue custom FX** (delay/reverb slot), not user oscillators — Phase 5 adds an FX lane and scaffolds.

## Current repo baseline

| Layer | Path | Notes |
|-------|------|--------|
| v1.1 (XD / NTS mkI / Prologue) | `src/oscillators/<folder>/` | 14 musical units + `template` + `wavetable` — **DSP implemented** |
| mkII (NTS-1 mkII) | `src/mkii/oscillators/<slug>/` | Scaffolds for catalog; **`tr808_kick_phonk` only `PORT_COMPLETE`** |
| Golden mkII reference | `tr808_kick_phonk` | Use for porting patterns |
| Agent workflow | `/logue-mkii-*` | [logue-mkii-workflow.md](logue-mkii-workflow.md) |

**Implication:** Legacy v1 sources under `src/oscillators/` are **reference ports** into mkII `osc.h`; new work ships **mkII-only** per owner decision.

---

## Request → slug mapping

| # | Your ask | Proposed slug | v1 source | mkII folder | Work type |
|---|----------|---------------|-----------|-------------|-----------|
| 0 | Dual-platform boilerplate | _(infra)_ | `oscillators/template` | `template_osc` | **Enhance** tooling + docs |
| 1 | Dusty Memphis sub-bass | `memphis_dust_sub` | _(new)_ | same | **New** (extend Juicy/EPS ideas) |
| 2 | Juno-style DCO | `juno_dco_osc` | port from `juno-rnb` | `juno_106_rnb_bass` exists | **Refocus** DCO osc vs full bass |
| 3 | Prophet VS — 128 waves → wavetable | `prophet_vs_wt128` | _(new)_ | _(new)_ | **New** + **data pipeline** |
| 4 | JP-8000 supersaw | `jp8000_supersaw` | _(new)_ | _(new)_ | **New** |
| 5 | Prophet-5 analog | `prophet5_analog` | port `prophet-funk` | `prophet5_funk_rnb_bass` | **Port + retune** |
| 6 | Oberheim OB-Xa analog | `obxa_analog` | _(new)_ | _(new)_ | **New** |
| 7 | PPG Wave 2 + Microwave wavetable | `ppg_microwave_wt` | _(new)_ | _(new)_ | **New** + tables |
| 8 | Phat Minimoog bass | `minimoog_phatt` | blend `sub-phatty` / `moog-voyager-se` | partial exists | **New** dedicated osc |
| 9 | SH-101 | `sh101_classic` | port `sh101-babyface` | `sh101_babyface_bass` | **Port** |
| 10 | Detroit rap TB-303 | `tb303_detroit_acid` | _(new)_ | _(new)_ | **New** |
| 11 | Korg DW-8000 | `korg_dw8000_dig` | _(new)_ | _(new)_ | **New** (DWGS **digital waves** primary) |
| 12 | Korg Poly-61 | `korg_poly61_dco` | _(new)_ | _(new)_ | **New** (DCO + simple poly char) |
| 13 | Prophet-6 / OB-6 | `prophet6_analog` + `ob6_analog` | _(new)_ | _(new)_ | **New** — **two separate units** (locked) |
| 14 | Ensoniq SQ-80 | `ensoniq_sq80_wt` | extend `ensoniq-eps1` ideas | _(new)_ | **New** + wavetable bank |
| 15 | Solina string ensemble | `solina_string_ensemble` | _(new)_ | _(new)_ | **New** (divide-down + ensemble) |
| 16 | Rage rap saw synth | `rage_rap_supersaw` | _(new)_ | _(new)_ | **New** (genre-tuned supersaw core) |
| 17 | Virus hypersaw | `virus_hypersaw` | _(new)_ | _(new)_ | **New** (shared supersaw engine) |
| 18 | Acoustic physical modeling (poly) | `acoustic_pm_poly` | _(new)_ | _(new)_ | **New** (scoped PM — see below) |
| 19 | TR-808 **4-voice drum** (kick/snare/CH/OH) | `tr808_drumkit_4voice` | extend `tr808-kick` | _(new)_ | **New** (multi-voice drum osc) |
| 20 | FM cowbell + perc | `fm808_cowbell_perc` | _(new)_ | _(new)_ | **New** (FM + modal perc) |
| 21 | SP-1200-style processing | `sp1200_fx` | _(new FX lane)_ | `src/mkii/fx/` | **New custom FX** |
| 22 | Dream / shimmer reverb | `dream_reverb_fx` | _(new FX lane)_ | `src/mkii/fx/` | **New custom FX** |

**Already close (rename optional, not duplicate work):**

- Memphis trunk sub → `memphis-juicy` / `juicy_j_three6_memphis_bass` (different vibe from “dust” — keep both).
- Moog family → `sub-phatty`, `moog-voyager-se`, `west-coast-moog` (Minimoog osc should be its own macro set).
- Ensoniq grit → `ensoniq-eps1` / `ensoniq_eps1_memphis_bass` (SQ-80 unit is **broader WT**, not Memphis-specific).
- 808 kick only → `tr808-kick` / `tr808_kick_phonk` (keep; **4-voice kit** is a separate loadable unit).

### Supersaw family (share one DSP core)

| Slug | Differentiator |
|------|----------------|
| `jp8000_supersaw` | Roland JP-8000: 7 voices, bright, trance/lead |
| `virus_hypersaw` | Access Virus: tighter spread, more HP, “hypersaw” density |
| `rage_rap_supersaw` | Preset-first: detuned saws + sub — **Playboi Carti** + **Trippie Redd** targets (locked) |

**Plan:** Implement `src/common/logue/supersaw_core.h` (or copy per unit if flash linking is awkward) once; three manifests / param curves.

---

## Phase 0 — Dual-platform boilerplate (do first)

**Goal:** One command creates a matched v1 + mkII tree with agent state and build hints.

**Today:**

- v1: copy `src/oscillators/template/` → `./tools/build.sh oscillators/<name> minilogue-xd`
- mkII: copy `template_osc` or `scaffold-mkii.py oscillators/<v1-folder>`

**Deliverables (planned):**

1. `tools/new-oscillator.sh <slug> [--from-v1 path]` — creates:
   - `src/oscillators/<kebab-name>/` with `manifest.json` `_agent` block
   - `src/mkii/oscillators/<slug>/` via scaffold or template copy
   - `.agent/status.json`, empty `brief.md` / `design.md`
2. `docs/logue-dual-platform-boilerplate.md` — parameter scaling (0–100 v1 ↔ 0–1023 mkII), build matrix
3. Optional shared header-only helpers under `src/common/logue/` (only if flash allows per-unit copy)

**Exit criteria:** New slug scaffolds; v1 builds for XD; mkII passes `osc-eval-mkii.sh --static-only`.

---

## Phase 1 — Ports + quick wins (reuse v1 DSP)

Low risk, validates pipeline on hardware.

| Slug | Priority | Notes |
|------|----------|--------|
| `sh101_classic` | P1 | Port `sh101_bass.cc` → mkII |
| `prophet5_analog` | P1 | Port `prophet_bass.cc`; widen lead/bass macros |
| `juno_dco_osc` | P1 | DCO+PWM from `juno_rnb.cc` + **on-board chorus**; no full bass LP/ADR rig |
| `memphis_dust_sub` | P1 | **Your accepted design** — see brief below |

**Each unit:** brief → design → impl → `./tools/osc-eval-mkii.sh <slug>` → `./tools/build-mkii.sh <slug>` → XD `./tools/build.sh`.

---

## Phase 2 — New algorithmic oscillators

| Slug | DSP sketch | Param budget (≤10) |
|------|------------|---------------------|
| `jp8000_supersaw` | 2–7 detuned saws + HP/LP tone; optional spread | Detune, Voices, Mix, Tone, Width, … |
| `obxa_analog` | Dual saw/pulse, cross-mod lite, OB-style filter tracking via host cutoff | Mix, PWM, Detune, Brute, … |
| `minimoog_phatt` | 3 osc stack (24′/16′/8′), sync optional, saturation | Osc mix, Glide, Drive, … |
| `tb303_detroit_acid` | Detroit rap **bassline:** saw/square, **slide + accent**, filter via host res/cutoff | Wave, Slide, Accent, EnvAmt, … |
| `korg_poly61_dco` | Single DCO: saw/pulse/PWM, mild drift; host filter for “Polysix-adjacent” tone | PWM, Drift, Wave, … |
| `prophet6_analog` | P6-style wavetable/analog blend, soft FM edges | Blend, Detune, … |
| `ob6_analog` | OB-6 SEM-style saws/pulse, spread | Detune, PWM, … |
| `solina_string_ensemble` | Divide-down square waves (16′/8′/4′ mix) + slow ensemble chorus + EQ tilt | Mix, Ensemble, Tone, … |
| `jp8000_supersaw` / `virus_hypersaw` / `rage_rap_supersaw` | Shared supersaw core (see table above) | Voices, Detune, Sub, Tone, … |
| `korg_dw8000_dig` | **Digital wave cycles** (DWGS-style tables, 8–16 waves), morph + grit; filter via host | Wave, Morph, Grit, … |

**mkII + v1:** Implement v1 first (proven pattern), scaffold mkII, port.

---

## Phase 3 — Wavetable oscillators (multi-product)

### A) `prophet_vs_wt128`

**User ask:** Find/extract all **128** VS single-cycle waves into one morphing wavetable osc.

**Constraints:**

- **Source (locked):** **Recreated VS-inspired waves** only (document in unit README).
- **Flash plan:** **64 samples/wave**; aim **128 waves**, ship **64-wave v1** if size gate fails; optional **Bank** param for 64+64 later.
- **Playback:** Linear interp + optional spectral morph between adjacent waves; `Wave` / `Morph` / `Character` params

### B) `ppg_microwave_wt`

**Goal:** PPG Wave 2 + Waldorf Microwave character in one morphable table.

- Curated set (16–32 waves, 64 samples): PPG-ish sync sweeps, Microwave digital edge — separate from SQ-80’s **48-wave** bank
- Share **wavetable tooling** with VS unit: `tools/wavetable/pack_tables.py` → `wt_bank.inc`
- Reference implementation pattern: `ensoniq-eps1` (multi-table, q31, 64-point)

### C) `ensoniq_sq80_wt`

**Goal:** SQ-80 **TransWave** / wavetable character (not a ROM dump of Ensoniq factory disks).

- **48 × 64** locked bank (vocal formants, bells, hybrid analog-digital) in one morph osc
- Reuse `pack_tables.py`; optional user WAV import (gitignored)
- Params: **Wave**, **Morph**, **FilterTrack** (follow host cutoff), **Edge**, **Attack** (WT start bias)
- Relationship: broader than `ensoniq-eps1` (Memphis bass); can share table **pack format** only

**Shared tooling (Phase 3 prep):**

```text
tools/wavetable/
  pack_tables.py      # CSV/WAV → q31 .inc
  import_vs128.py     # user-supplied dump only (gitignored input)
  validate_size.sh    # arm-none-eabi-size gate vs budget
```

---

## Phase 4 — Physical modeling (scoped)

**Slug:** `acoustic_pm_poly`

A full **polyphonic** acoustic model (multiple strings/bodies, independent excitation) is **not realistic** in one logue **monophonic user osc** slot. Scoped deliverable:

| Mode | Technique | Use |
|------|-----------|-----|
| **Pluck** | Karplus-Strong / waveguide lite, 1–2 delay lines | Guitar, harp-ish |
| **Strike** | Short noise burst + resonator | Mallet / piano-ish attack |
| **Bow** (v2 only) | Continuous excitation + friction | Drop if CPU over budget |

**v1 (locked):** **Pluck + Strike** both ship — **Mode** (or **Excite**) param switches engines; shared **Decay / Damping / Tone**.

**Params (≤10):** Mode, Decay, Damping, Tone, Body, Bright, StrikeHard, … Host **cutoff** = fingerboard / body damping.

**“Poly” in the name:** Document as **polyphonic-friendly timbre** (long decay, chord stacks on external poly synth), not internal polyphony. If you need true poly PM, that belongs in a **multi-voice instrument** (JUCE/HISE lane), not NTS-1 single osc.

**Risk:** CPU on mkI v1.1 — profile early; simplify delay length on nutekt-digital if needed.

---

## Phase 5 — Drums, FM perc, and custom FX

### Hardware reality (NTS-1 / mkII / XD)

| Slot | What loads | Your ask |
|------|------------|----------|
| **User oscillator** | One custom osc at a time (monophonic output per note) | 4-voice 808 kit, FM cowbell/perc |
| **User delay / reverb (FX)** | One custom FX at a time (mkII / supported platforms) | SP-1200 color, dream reverb |

Oscillators **cannot** replace the FX slot — plan **two FX binaries** plus drum/FM **osc** binaries. On mkI NTS-1, FX custom units depend on SDK/platform support (verify in logue-sdk `platform/*/custom_fx` README when implementing).

### A) `tr808_drumkit_4voice` (oscillator)

**Goal:** One loadable unit with **four 808 voices**: bass drum, snare, closed hat, open hat.

**Voice selection (locked):** **MIDI note map** — each drum on its own key (pad-friendly). Document map in `manifest.json` + `.agent/brief.md`.

| Drum | Default MIDI note |
|------|-------------------|
| Kick | **C1** (36) |
| Snare | **D1** (38) |
| Closed hat | **F#1** (42) |
| Open hat | **A#1** (46) |

Optional later: **Voice** param override for one-key “selector” mode — not v1.

**Shared macros (≤10):** **Tone**, **Decay**, **Punch**, **Snare Snap**, **Hat Length**, **OH Decay**, **Drive**, **Level**, **Tune**, **Accent** (or spare for kit-wide tone).

**DSP sketch:**

- **Kick:** Reuse/port `kick808.cc` synthesis (sine sweep + clip).
- **Snare:** Tone + noise burst, optional body tone ~180 Hz.
- **CH:** Short filtered noise + weak square click.
- **OH:** Longer noise + decay; shared noise gen, no alloc.

**CPU:** One voice active per note event (monophonic legato off); overlapping hats may need **steal** or **short parallel env** — profile on hardware.

**Relationship:** Keep `tr808_kick_phonk` as **kick-only** phonk specialist; kit is general 808 drums.

### B) `fm808_cowbell_perc` (oscillator)

**Goal:** FM and metallic perc — **808 cowbell**, rimshot-adjacent click, hand clave / tunable bell family.

**Voice map (similar to kit):**

| Voice | Engine |
|-------|--------|
| Cowbell | 2-op FM (fixed ratio ~1.75:1 style), short decay |
| Metal perc | FM or ring-mod sine pairs |
| Clave / stick | Sine + fast env |

**Params:** **Voice** or MIDI map, **FM Index**, **Ratio**, **Decay**, **Tone**, **Metal**, **Level**, …

**Locked:** **Always separate** from the 4-voice kit — you get **both** `tr808_drumkit_4voice` and `fm808_cowbell_perc` as their own `.prg` / `.nts1mkiiunit` files. NTS still loads **one osc at a time**; swap units when you switch from kit to cowbell/perc.

### C) `sp1200_fx` (custom FX — not an oscillator)

**Goal:** **Emu SP-1200** vibe on incoming audio — **locked emphasis: bit crush + sample-rate reduction** (12-bit / ~26 kHz feel). Smear/saturation are secondary.

**FX type:** Custom **delay** or **reverb** slot (implementation follows logue-sdk **Fx** / `unit_fx` pattern — mirror `dummy-delay` in SDK).

**Params (≤10):** **Crush**, **SampleRate**, **Drive**, **Mix**, **Output**, … (minimal **Smear** / width — low default)

**Repo:** `src/mkii/fx/sp1200_fx/` — **mkII only** (no nutekt v1 FX path planned).

**Note:** Separate **SP-1200 sampler emulation** exists elsewhere in the monorepo (JUCE lane); this FX unit is **color only**, not sampling.

### D) `dream_reverb_fx` (custom FX)

**Goal:** “Dream” reverb — **locked default: bright shimmer** (octave / +5th in feedback, airy tail). Optional **Dark** param pulls toward dub; factory presets start bright.

**Engine:** Schroeder/Moorer-lite + **pitch-shifted feedback tap** (grain or all-pass diffusion); cap delay lines for MCU.

**Params:** **Size**, **Decay**, **Shimmer**, **Bright**, **Mod**, **PreDelay**, **Mix**, **Tone**, …

**Repo:** `src/mkii/fx/dream_reverb_fx/`

### Phase 5 infra (planned)

```text
tools/mkii/scaffold-mkii-fx.py     # from logue-sdk dummy-delay / dummy-reverb
tools/build-mkii-fx.sh <slug>
docs/logue-custom-fx-lane.md       # FX vs osc, load order on NTS
```

**Agent:** Extend orchestrator or add `.cursor/agents/logue-mkii-fx/` when FX work starts.

---

## Dusty Memphis sub-bass — design brief (accepted)

**Slug:** `memphis_dust_sub`  
**Target:** NTS-1 mkII  
**Role:** Sub layer under 808 kick; Three 6 / Memphis rap trunk, **more lo-fi and “dust”** than `memphis-juicy`.

| Param | Intent |
|-------|--------|
| Vibe | Overall darkness + level |
| Sub | Fundamental sine/trapezoid weight |
| Trunk | Octave / fifth body (~25–45 Hz feel) |
| Dust | Noise + bit-crush / sample-rate reduction |
| Tape | Soft clip + slow compression |
| Mid | Upper harmonics for small speakers |
| Glide / ADR | Same family as Juicy unit |

**DSP notes:** Phase-locked subs; avoid mud with high-pass on dust bus; no malloc in `OSC_CYCLE` / `process()`.

**Presets (draft):** “Trunk dusty”, “Lo-fi trunk”, “Phone mid”, “DJ Paul rumble”.

---

## Decisions locked (owner)

| Topic | Decision |
|-------|----------|
| 808 kit voice pick | **MIDI note map** (C1/D1/F#1/A#1 defaults) |
| Drum + FM perc | **Two separate oscillators** — both shipped |
| Rage rap supersaw | Preset targets: **Playboi Carti**, **Trippie Redd** |
| Supersaw priority | **`virus_hypersaw` first** after supersaw core (before JP-8000 / rage) |
| Prophet-6 / OB-6 | **Two units:** `prophet6_analog`, `ob6_analog` |
| Platform | **mkII-only** for new osc + FX (no XD v1.1 ship target) |
| Dream reverb | **Bright shimmer** factory default |
| SP-1200 FX | **Bit crush / sample-rate** primary; smear secondary |
| Prophet VS waves | **Recreated / VS-inspired** — no ROM in repo |
| Juno-106 DCO scope | **Standalone DCO osc** (`juno_dco_osc`) — see below |
| TB-303 | **Detroit rap bassline** — saw/square + **accent + slide** in unit |
| SQ-80 table | **48 waves × 64 samples** (q31), morph between indices |
| Acoustic PM v1 | **Pluck + Strike** modes in one unit |
| DW-8000 | **Digital wave cycles** primary (not analog filter emulation in osc) |

### Juno-106 DCO scope (locked)

Two units, two jobs:

| Slug | Role |
|------|------|
| `juno_dco_osc` | **Juno-106 DCO voice:** saw + pulse/PWM, mild drift, **built-in chorus** (port from `juno_rnb.cc` chorus path). **No** full 24 dB LP stack or bass-oriented ADR rig — use NTS **cutoff/res** for filter sweeps. |
| `juno_106_rnb_bass` | **Full bass voice:** port of `juno-rnb` (env, chorus, LP) — 80s R&B / bass lines. |

Do not merge into one binary; params stay under 10 per unit. **`juno_dco_osc` params (draft):** Vibe, Saw, PWM, Chorus, Glide, … (no Attack/Decay/Release block).

### DW-8000 digital waves (locked)

**Slug:** `korg_dw8000_dig`

- Emphasis on **short digital single-cycle / DWGS-style tables** and **wave morph**, not SSM-style filter emulation inside the osc.
- **Grit / edge** params for lo-fi digital character; musical filtering = **host cutoff/res**.
- 8–16 internal waves; reuse wavetable pack tooling when available.

### TB-303 Detroit rap bassline (locked)

**Slug:** `tb303_detroit_acid`

- **Wave:** saw / square (303-style)
- **Slide:** portamento between legato notes (essential for Detroit / electro-rap 303 lines)
- **Accent:** velocity or auto-accent envelope bump on selected notes
- **Filter:** track **host cutoff + resonance** (squawk); optional internal env **Amount** param
- **Presets:** “Detroit slide”, “Fast 16ths”, “Reso rip”, “Sub 303 under kick”

### SQ-80 table size (locked)

| Setting | Value | Flash (approx.) |
|---------|-------|-----------------|
| Waves | **48** curated TransWave-style | — |
| Samples / wave | **64** (power-of-two, EPS pattern) | 48 × 64 × 4 B ≈ **12 KiB** q31 |
| Morph | Adjacent-wave crossfade + **Wave** index | headroom for CPU |

If a unit overflows flash after `arm-none-eabi-size`, drop to **32 × 64** before shrinking sample length.

### Prophet VS128 (recreated waves)

- **No Sequential ROM** in tree; procedural + curated **VS-inspired** single cycles
- Target **64–128 waves** with same **64-sample** cycles (128 × 64 × 4 B ≈ 32 KiB) — ship **64-wave v1** if 128 exceeds flash; second bank param later

---

## Recommended delivery order

```text
Phase 0   boilerplate + roadmap
Phase 1a  memphis_dust_sub
Phase 1b  sh101_classic, prophet5_analog, juno_dco_osc (ports)
Phase 1c  supersaw core → virus_hypersaw          ← priority (owner)
Phase 2a  minimoog_phatt, tb303_detroit_acid, obxa_analog, korg_poly61_dco
Phase 2b  jp8000_supersaw → rage_rap_supersaw     ← Carti / Trippie presets
Phase 2c  solina_string_ensemble, prophet6_analog, ob6_analog, korg_dw8000_dig
Phase 3   wavetable tooling → prophet_vs_wt128 → ppg_microwave_wt → ensoniq_sq80_wt
Phase 4   acoustic_pm_poly (after CPU baseline from Phase 2)
Phase 5a  tr808_drumkit_4voice (+ port kick DSP), fm808_cowbell_perc
Phase 5b  FX lane bootstrap → sp1200_fx → dream_reverb_fx
```

Parallel track: **mkII ports** for every v1 unit touched.

**Drums priority option:** If you want kits before analog synths, move **Phase 5a** up after Phase 1 (kick port already exists).

### Collection docs (when implementing)

Split catalog for clarity:

| Doc | Contents |
|-----|----------|
| [nts1-multi-bass-oscillators.md](nts1-multi-bass-oscillators.md) | Bass / sub / 303 / Memphis |
| `docs/logue-oscillator-catalog.md` _(planned)_ | Leads, supersaws, strings, PM, WT, drums/FM perc |
| `docs/logue-custom-fx-lane.md` _(planned)_ | SP-1200, dream reverb, future FX |

---

## Per-unit agent checklist

For each slug:

1. `.agent/brief.md` + `.agent/design.md`
2. v1 impl under `src/oscillators/`
3. `manifest.json` with `_agent.presets` (0–100)
4. mkII port `osc.h` marked `PORT_COMPLETE` when done
5. `./tools/osc-eval-mkii.sh <slug>`
6. Row in [nts1-multi-bass-oscillators.md](nts1-multi-bass-oscillators.md) or new **collection** doc for non-bass oscs
7. `dev_id` / `unit_id` before public release

---

## Open questions (remaining)

_(None — collection spec locked; implementation continues per phase order.)_

---

## Related

- [logue-mkii-oscillator-agent.md](logue-mkii-oscillator-agent.md)
- [nts1-multi-bass-oscillators.md](nts1-multi-bass-oscillators.md)
- [platform-matrix.md](platform-matrix.md)
