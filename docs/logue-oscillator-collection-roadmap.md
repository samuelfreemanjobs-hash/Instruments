# logue custom oscillator collection — roadmap

**Owner intent:** NTS-1 mkII (primary) + Minilogue XD (v1.1 `.prg` where applicable).  
**Status:** Planning (2026-09-22, expanded). **22 oscillator products** + Phase 0 infra. Update as slugs ship.

## Current repo baseline

| Layer | Path | Notes |
|-------|------|--------|
| v1.1 (XD / NTS mkI / Prologue) | `src/oscillators/<folder>/` | 14 musical units + `template` + `wavetable` — **DSP implemented** |
| mkII (NTS-1 mkII) | `src/mkii/oscillators/<slug>/` | Scaffolds for catalog; **`tr808_kick_phonk` only `PORT_COMPLETE`** |
| Golden mkII reference | `tr808_kick_phonk` | Use for porting patterns |
| Agent workflow | `/logue-mkii-*` | [logue-mkii-workflow.md](logue-mkii-workflow.md) |

**Implication:** Most of your list can **reuse v1 DSP** on XD today; mkII needs **ports** (or greenfield `osc.h`) per unit.

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
| 11 | Korg DW-8000 | `korg_dw8000_dig` | _(new)_ | _(new)_ | **New** (DWGS + analog-ish edge) |
| 12 | Korg Poly-61 | `korg_poly61_dco` | _(new)_ | _(new)_ | **New** (DCO + simple poly char) |
| 13 | Prophet-6 / OB-6 | `sequential_p6_ob6` | _(new)_ | _(new)_ | **New** (dual-personality macro) |
| 14 | Ensoniq SQ-80 | `ensoniq_sq80_wt` | extend `ensoniq-eps1` ideas | _(new)_ | **New** + wavetable bank |
| 15 | Solina string ensemble | `solina_string_ensemble` | _(new)_ | _(new)_ | **New** (divide-down + ensemble) |
| 16 | Rage rap saw synth | `rage_rap_supersaw` | _(new)_ | _(new)_ | **New** (genre-tuned supersaw core) |
| 17 | Virus hypersaw | `virus_hypersaw` | _(new)_ | _(new)_ | **New** (shared supersaw engine) |
| 18 | Acoustic physical modeling (poly) | `acoustic_pm_poly` | _(new)_ | _(new)_ | **New** (scoped PM — see below) |

**Already close (rename optional, not duplicate work):**

- Memphis trunk sub → `memphis-juicy` / `juicy_j_three6_memphis_bass` (different vibe from “dust” — keep both).
- Moog family → `sub-phatty`, `moog-voyager-se`, `west-coast-moog` (Minimoog osc should be its own macro set).
- Ensoniq grit → `ensoniq-eps1` / `ensoniq_eps1_memphis_bass` (SQ-80 unit is **broader WT**, not Memphis-specific).

### Supersaw family (share one DSP core)

| Slug | Differentiator |
|------|----------------|
| `jp8000_supersaw` | Roland JP-8000: 7 voices, bright, trance/lead |
| `virus_hypersaw` | Access Virus: tighter spread, more HP, “hypersaw” density |
| `rage_rap_supersaw` | Preset-first: detuned saws + sub, 808-friendly, modern rage rap |

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
| `juno_dco_osc` | P1 | Strip/enhance DCO+PWM from `juno_rnb.cc`; less “full bass rig”, more pure DCO |
| `memphis_dust_sub` | P1 | **Your accepted design** — see brief below |

**Each unit:** brief → design → impl → `./tools/osc-eval-mkii.sh <slug>` → `./tools/build-mkii.sh <slug>` → XD `./tools/build.sh`.

---

## Phase 2 — New algorithmic oscillators

| Slug | DSP sketch | Param budget (≤10) |
|------|------------|---------------------|
| `jp8000_supersaw` | 2–7 detuned saws + HP/LP tone; optional spread | Detune, Voices, Mix, Tone, Width, … |
| `obxa_analog` | Dual saw/pulse, cross-mod lite, OB-style filter tracking via host cutoff | Mix, PWM, Detune, Brute, … |
| `minimoog_phatt` | 3 osc stack (24′/16′/8′), sync optional, saturation | Osc mix, Glide, Drive, … |
| `tb303_detroit_acid` | Saw/square, accent envelope, slide, **filter emphasis via host res/cutoff** | Wave, Env, Slide, Accent, … |
| `korg_poly61_dco` | Single DCO: saw/pulse/PWM, mild drift; host filter for “Polysix-adjacent” tone | PWM, Drift, Wave, … |
| `sequential_p6_ob6` | **Mode param:** P6 wavetable/analog blend vs OB-6 SEM-style saws; shared envelope hooks | Mode, Blend, Detune, … |
| `solina_string_ensemble` | Divide-down square waves (16′/8′/4′ mix) + slow ensemble chorus + EQ tilt | Mix, Ensemble, Tone, … |
| `jp8000_supersaw` / `virus_hypersaw` / `rage_rap_supersaw` | Shared supersaw core (see table above) | Voices, Detune, Sub, Tone, … |
| `korg_dw8000_dig` | Short **DWGS-style** cycle tables (8–16 waves) + digital grit; morph between waves | Wave, Morph, Grit, … |

**mkII + v1:** Implement v1 first (proven pattern), scaffold mkII, port.

---

## Phase 3 — Wavetable oscillators (multi-product)

### A) `prophet_vs_wt128`

**User ask:** Find/extract all **128** VS single-cycle waves into one morphing wavetable osc.

**Constraints:**

- **Legal / source:** Prophet VS ROM is copyrighted. The repo cannot ship extracted Sequential ROM. Acceptable paths:
  1. **You provide** a licensed dump or your own captures → `tools/wavetable/import_vs128.py` → `.inc` blobs
  2. **Recreate** a subset of classic VS waves procedurally (document as “inspired by”, not “VS ROM”)
  3. Hybrid: 32–64 recreated + user slot for custom waves
- **Flash size:** 128 × 256 × 4 B ≈ 128 KiB raw q31 — likely **too large** for one unit. Plan:
  - **64 samples/wave** (like EPS unit) → ~32 KiB for 128 waves, or
  - **128 waves × 128 samples**, or
  - **Two banks** (64+64) selected by param
- **Playback:** Linear interp + optional spectral morph between adjacent waves; `Wave` / `Morph` / `Character` params

### B) `ppg_microwave_wt`

**Goal:** PPG Wave 2 + Waldorf Microwave character in one morphable table.

- Smaller curated set (16–32 waves): PPG-ish sync sweeps, wavetable 64 partials, Microwave digital edge
- Share **wavetable tooling** with VS unit: `tools/wavetable/pack_tables.py` → `wt_bank.inc`
- Reference implementation pattern: `ensoniq-eps1` (multi-table, q31, 64-point)

### C) `ensoniq_sq80_wt`

**Goal:** SQ-80 **TransWave** / wavetable character (not a ROM dump of Ensoniq factory disks).

- Curated **24–48 waves** (vocal formants, bells, hybrid analog-digital) in one morph osc
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

| Mode (string param or fixed build) | Technique | Use |
|-----------------------------------|-----------|-----|
| **Pluck** (default) | Karplus-Strong / waveguide lite, 1–2 delay lines | Guitar, harp-ish |
| **Strike** | Short noise burst + resonator | Mallet / piano-ish attack |
| **Bow** (optional v2) | Continuous excitation + friction | Slower CPU; may drop if over budget |

**Params (≤10):** Excite, Decay, Damping, Tone, Body, Bright, … Host **cutoff** = fingerboard / body damping.

**“Poly” in the name:** Document as **polyphonic-friendly timbre** (long decay, chord stacks on external poly synth), not internal polyphony. If you need true poly PM, that belongs in a **multi-voice instrument** (JUCE/HISE lane), not NTS-1 single osc.

**Risk:** CPU on mkI v1.1 — profile early; simplify delay length on nutekt-digital if needed.

---

## Dusty Memphis sub-bass — design brief (accepted)

**Slug:** `memphis_dust_sub`  
**Target:** NTS-1 mkII + XD  
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

## Recommended delivery order

```text
Phase 0   boilerplate + roadmap
Phase 1a  memphis_dust_sub
Phase 1b  sh101_classic, prophet5_analog, juno_dco_osc (ports)
Phase 2a  minimoog_phatt, tb303_detroit_acid, obxa_analog, korg_poly61_dco
Phase 2b  supersaw core → jp8000_supersaw → virus_hypersaw → rage_rap_supersaw
Phase 2c  solina_string_ensemble, sequential_p6_ob6, korg_dw8000_dig
Phase 3   wavetable tooling → prophet_vs_wt128 → ppg_microwave_wt → ensoniq_sq80_wt
Phase 4   acoustic_pm_poly (after CPU baseline from Phase 2)
```

Parallel track: **mkII ports** for every v1 unit touched.

### Collection docs (when implementing)

Split catalog for clarity:

| Doc | Contents |
|-----|----------|
| [nts1-multi-bass-oscillators.md](nts1-multi-bass-oscillators.md) | Bass / sub / 303 / Memphis |
| `docs/logue-oscillator-catalog.md` _(planned)_ | Leads, supersaws, strings, PM, WT |

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

## Open questions (for your brainstorm)

1. **Minilogue XD:** Ship every unit on XD, or mkII-only for wavetable-heavy ones?
2. **Prophet VS:** Will you supply ROM/dump, or OK with “VS-inspired” recreated waves?
3. **Juno DCO:** Standalone DCO (osc only) vs keep full `juno-rnb` bass envelope/filter in the osc slot?
4. **303:** Pure osc (saw/square) vs include **internal accent/slide** (uses more CPU)?
5. **P6 vs OB-6:** One unit with **Mode** knob vs two separate `.prg` / `.nts1mkiiunit` files?
6. **SQ-80:** Full TransWave ambition vs **32 curated** waves for flash limits?
7. **Rage rap saw:** Reference tracks / artists for preset targets (e.g. early Carti vs current hyperpop)?
8. **Acoustic PM:** Pluck-only v1 OK, or **Strike** required for v1?
9. **DW-8000:** Emphasis on **digital wave cycles** vs **slammed SSM filter** (host filter does most filter work)?
10. **Priority tweak:** Any of the new eight ahead of Phase 2a (e.g. Virus or Solina before OB-Xa)?

---

## Related

- [logue-mkii-oscillator-agent.md](logue-mkii-oscillator-agent.md)
- [nts1-multi-bass-oscillators.md](nts1-multi-bass-oscillators.md)
- [platform-matrix.md](platform-matrix.md)
