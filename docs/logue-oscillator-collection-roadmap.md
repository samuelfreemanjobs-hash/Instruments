# logue custom oscillator collection — roadmap

**Owner intent:** NTS-1 mkII (primary) + Minilogue XD (v1.1 `.prg` where applicable).  
**Status:** Planning (2026-09-22). Update as slugs ship.

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

**Already close (rename optional, not duplicate work):**

- Memphis trunk sub → `memphis-juicy` / `juicy_j_three6_memphis_bass` (different vibe from “dust” — keep both).
- Moog family → `sub-phatty`, `moog-voyager-se`, `west-coast-moog` (Minimoog osc should be its own macro set).

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

**mkII + v1:** Implement v1 first (proven pattern), scaffold mkII, port.

---

## Phase 3 — Wavetable oscillators (two products)

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

**Shared tooling (Phase 3 prep):**

```text
tools/wavetable/
  pack_tables.py      # CSV/WAV → q31 .inc
  import_vs128.py     # user-supplied dump only (gitignored input)
  validate_size.sh    # arm-none-eabi-size gate vs budget
```

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
Phase 0  boilerplate + roadmap (this doc)
Phase 1a memphis_dust_sub (new, your priority)
Phase 1b sh101_classic, prophet5_analog, juno_dco_osc (ports)
Phase 2  jp8000_supersaw, minimoog_phatt, tb303_detroit_acid, obxa_analog
Phase 3  wavetable tooling → prophet_vs_wt128 → ppg_microwave_wt
```

Parallel track: **mkII ports** for any v1 unit touched in Phase 1–2.

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
5. **Collection naming:** Second doc `docs/logue-oscillator-catalog.md` for supersaw/303/wavetable (non-bass)?

---

## Related

- [logue-mkii-oscillator-agent.md](logue-mkii-oscillator-agent.md)
- [nts1-multi-bass-oscillators.md](nts1-multi-bass-oscillators.md)
- [platform-matrix.md](platform-matrix.md)
