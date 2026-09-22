# logue custom oscillator collection — roadmap

**Owner intent:** NTS-1 mkII (primary) + Minilogue XD (v1.1 `.prg` where applicable).  
**Status:** Planning (2026-09-22, expanded). **24 oscillator products** + **2 custom FX units** + Phase 0 infra. Update as slugs ship.

**Scope split:** This repo’s agent tooling today is **osc-first** (`src/mkii/oscillators/`). **SP-1200** and **dream reverb** are **logue custom FX** (delay/reverb slot), not user oscillators — Phase 5 adds an FX lane and scaffolds.

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
| 13 | Prophet-6 / OB-6 | `sequential_p6_ob6` _(or split later)_ | _(new)_ | _(new)_ | **New** (dual-personality or **2 units** — TBD) |
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

## Phase 5 — Drums, FM perc, and custom FX

### Hardware reality (NTS-1 / mkII / XD)

| Slot | What loads | Your ask |
|------|------------|----------|
| **User oscillator** | One custom osc at a time (monophonic output per note) | 4-voice 808 kit, FM cowbell/perc |
| **User delay / reverb (FX)** | One custom FX at a time (mkII / supported platforms) | SP-1200 color, dream reverb |

Oscillators **cannot** replace the FX slot — plan **two FX binaries** plus drum/FM **osc** binaries. On mkI NTS-1, FX custom units depend on SDK/platform support (verify in logue-sdk `platform/*/custom_fx` README when implementing).

### A) `tr808_drumkit_4voice` (oscillator)

**Goal:** One loadable unit with **four 808 voices**: bass drum, snare, closed hat, open hat.

**Voice selection (locked):** **`Voice` param** (0–3) — kick / snare / closed hat / open hat. MIDI pitch still sets **tuning** and envelope feel per hit.

| `Voice` | Drum |
|---------|------|
| 0 | Kick |
| 1 | Snare |
| 2 | Closed hat |
| 3 | Open hat |

Optional later: **hybrid** preset in docs (param + note map) for pad controllers — not v1 default.

**Shared macros (≤10):** **Voice**, **Tone**, **Decay**, **Punch**, **Snare Snap**, **Hat Length**, **OH Decay**, **Drive**, **Level**, **Tune**.

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

**Goal:** **Emu SP-1200** vibe on incoming audio: 12-bit crunch, ~26 kHz bandwidth feel, mono-ish image, short **micro-delay** smear, gentle **input saturation**.

**FX type:** Custom **delay** or **reverb** slot (implementation follows logue-sdk **Fx** / `unit_fx` pattern — mirror `dummy-delay` in SDK).

**Params (≤10):** **Crush**, **SampleRate**, **Drive**, **Width**, **Smear**, **Mix**, **Output**, …

**Repo:** `src/mkii/fx/sp1200_fx/` (+ optional v1 `src/fx/sp1200/` for nutekt if supported).

**Note:** Separate **SP-1200 sampler emulation** exists elsewhere in the monorepo (JUCE lane); this FX unit is **color only**, not sampling.

### D) `dream_reverb_fx` (custom FX)

**Goal:** “Dream” reverb — long tail, **shimmer** (octave/+5th in feedback), dark/modulated wash (Valhalla-style **inspired**, not a clone).

**Engine:** Schroeder/Moorer-lite + **pitch-shifted feedback tap** (grain or all-pass diffusion); cap delay lines for MCU.

**Params:** **Size**, **Decay**, **Shimmer**, **Dark**, **Mod**, **PreDelay**, **Mix**, **Tone**, …

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

## Decisions locked (owner)

| Topic | Decision |
|-------|----------|
| 808 kit voice pick | **`Voice` param** (0–3), not MIDI-map-first |
| Drum + FM perc | **Two separate oscillators** — both shipped |
| Rage rap supersaw | Preset targets: **Playboi Carti**, **Trippie Redd** |
| Supersaw priority | **`virus_hypersaw` first** after supersaw core (before JP-8000 / rage) |

---

## Recommended delivery order

```text
Phase 0   boilerplate + roadmap
Phase 1a  memphis_dust_sub
Phase 1b  sh101_classic, prophet5_analog, juno_dco_osc (ports)
Phase 1c  supersaw core → virus_hypersaw          ← priority (owner)
Phase 2a  minimoog_phatt, tb303_detroit_acid, obxa_analog, korg_poly61_dco
Phase 2b  jp8000_supersaw → rage_rap_supersaw     ← Carti / Trippie presets
Phase 2c  solina_string_ensemble, sequential_p6_ob6, korg_dw8000_dig
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

1. **Minilogue XD:** Ship every unit on XD, or mkII-only for wavetable-heavy ones?
2. **Prophet VS:** Will you supply ROM/dump, or OK with “VS-inspired” recreated waves?
3. **Juno DCO:** Standalone DCO (osc only) vs keep full `juno-rnb` bass envelope/filter in the osc slot?
4. **303:** Pure osc (saw/square) vs include **internal accent/slide** (uses more CPU)?
5. **P6 vs OB-6:** One unit with **Mode** knob vs **two separate** loadables?
6. **SQ-80:** Full TransWave ambition vs **32 curated** waves for flash limits?
7. **Acoustic PM:** Pluck-only v1 OK, or **Strike** required for v1?
8. **DW-8000:** Emphasis on **digital wave cycles** vs **slammed SSM filter** (host filter does most filter work)?
9. **FX target:** mkII only first, or Minilogue XD / NTS mkI custom FX too (platform-dependent)?
10. **Dream reverb:** More **shimmer/ambient** or **dark dub** default?
11. **SP-1200 FX:** Emphasis on **bit/sample-rate** vs **short delay smear** (classic SP “feel”)?

---

## Related

- [logue-mkii-oscillator-agent.md](logue-mkii-oscillator-agent.md)
- [nts1-multi-bass-oscillators.md](nts1-multi-bass-oscillators.md)
- [platform-matrix.md](platform-matrix.md)
