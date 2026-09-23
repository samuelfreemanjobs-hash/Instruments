# Minilogue XD — 4-op FM oscillator + DX7 patches (plan)

**Slugs (v1.1):**

| Slug | Panel name | Operators |
|------|------------|-----------|
| `fm4op-dx7-xd` | FM4 DX7 | 4-op (FM4OSC-style) |
| **`fm6op-dx7-xd`** | **FM6OSC** | **6-op (full DX7 voice target)** |

**Target:** [Minilogue XD](https://www.korg.com/miniloguexd/) custom oscillator (logue SDK **1.1-0**, Cortex-M4).  
**Inspiration:** Korg **FM4OSC** / extended **6-op** FM — **not clones**; our engine + **Yamaha DX7** import.

---

## FM6OSC (`fm6op-dx7-xd`)

**Yes — we can make FM6OSC.** This is the right choice if your library is **classic DX7 6-operator** patches.

| Topic | FM4 (`fm4op-dx7-xd`) | **FM6OSC** (`fm6op-dx7-xd`) |
|-------|----------------------|-------------------------------|
| DX7 patch fit | Subset / folded | **Primary target** |
| CPU on XD | Lower | **Higher** — use minimal FX; profile polyphony |
| Code in repo | `src/oscillators/fm4op-dx7-xd/` | `src/oscillators/fm6op-dx7-xd/` |
| Reference | FM48 | **[FM64](https://github.com/dukesrg/logue-osc)** |

**v0 shipped in repo:** 6-op engine, DX7 **algorithm 5** proxy + fallback; **Patch** bank import same pipeline as FM4 (`tools/dx7/import_dx7_bank.py` → future `Voice6` mapper).

**Build:**

```bash
./tools/build.sh oscillators/fm6op-dx7-xd minilogue-xd
```

**Separate from:** mkII collection (`src/mkii/`) and [external reference repos](logue-external-reference-repos.md) (study only unless licensed port).

---

## Is it possible?

| Goal | Verdict | Notes |
|------|---------|--------|
| **4-op FM** custom osc on Minilogue XD | **Yes** | Same SDK lane as NTS-1 mkI / XD; CPU is **tighter** than mkII — expect fewer ops / simpler envelopes than desktop FM. |
| **Load “a ton of” DX7 patches** | **Yes, with tooling** | Patches ship as **embedded voice banks** inside the `.prg` / unit blob (build time), not infinite SD-card library. |
| **Drop-in FM4OSC / microKORG patches** | **No** | FM4OSC uses Korg’s closed patch format; not DX7 SysEx. |
| **Bit-accurate DX7 on 4 operators** | **No** | DX7 is **6-op**, **32 algorithms**. 4-op can cover **many** sounds (DX21/DX11 family) but not all DX7 voices faithfully. |
| **Live SysEx dump from DX7 editor into XD while playing** | **Hard / non-standard** | logue user osc has **no official “receive DX7 sysex” API**. Practical path: **convert `.syx` → bank file → rebuild osc** (same idea as [dukesrg FM64.sh](../reference/logue/logue-osc/FM64.sh)). |

**Recommendation:** Treat **dukesrg [FM48](https://github.com/dukesrg/logue-osc)** (4-op) / **[FM64](https://github.com/dukesrg/logue-osc)** (6-op) as **reference implementations** for bank injection and parameter mapping; build **our** `fm4op-dx7-xd` if you want FM4OSC-style UX under your dev_id and a smaller, maintainable codebase.

---

## Architecture (proposed)

```text
Your DX7 .syx banks
    → tools/dx7/import_dx7_bank.py (parse 4096-byte voice, map to Voice4 or Voice6)
    → payload.bin / custom_data section in manifest
    → fm4op-dx7-xd.prg or fm6op-dx7-xd.prg (minilogue-xd)
    → Panel: Patch (1…N), Algorithm, Velocity (AC), Tone, …
```

### Voice model (internal `Voice4`)

Compact runtime struct (flash-backed):

- 4 operators: ratio (coarse/fine), level, EG rates/levels (simplified), feedback
- Algorithm index (subset of **opsix-style 40 algorithms** or DX11 4-op set — FM48 note)
- Per-voice transpose / velocity sensitivity (DX7 KVS → scalar)

Full DX7 156-byte voice → **lossy mapper** (drop op5/op6 or fold into mod indices).

### Panel parameters (v1 target, ≤6 manifest + custom_params later)

| Param | Role |
|-------|------|
| **Patch** | Select voice 1…32 (or 1…64 if flash allows) |
| **Algo** | Algorithm override / offset |
| **Level** | Master output |
| **Velo** | Maps from XD **assignable controller** (logue has no per-note velocity to osc — FM64 pattern) |
| **Bright** | Quick tone / output level scale |
| **Detune** | Global detune |

### CPU / polyphony (realistic)

- XD shares CPU with **2 digital osc slots + FX**. A 4-op FM voice is **~4–8 sin + envelopes** per sample.
- Start **monophonic-equivalent** engine (one note per voice slot — XD poly uses multiple instances).
- Profile with **FX off**, then add reverb/delay; degrade gracefully (skip op updates, lower envelope rate).

---

## DX7 patch pipeline (phases)

| Phase | Deliverable |
|-------|-------------|
| **P0** | This plan + scaffold `src/oscillators/fm4op-dx7-xd/` (minimal 4-op proof) |
| **P1** | `tools/dx7/import_dx7_bank.py` — ingest standard **32-voice** DX7 bank sysex → `voices.bin` |
| **P2** | Map **DX7 → Voice4** (document unsupported algorithms; list “good” patches) |
| **P3** | Embed **4× 32-voice banks** (128 voices) if flash budget OK on M4 |
| **P4** | Optional: **6-op** mode or mkII port (`dx7_fm_bass` roadmap) for heavier patches |

**Reference tooling (local clone):** `reference/logue/logue-osc/FM64.sh` injects up to **4×4096-byte** banks into FM64 payload.

---

## FM4OSC vs this project

| | microKORG 2 FM4OSC | `fm4op-dx7-xd` |
|--|-------------------|----------------|
| Operators | 4 (Korg) | 4 (ours) |
| Patch source | Korg FM4OSC presets | **Your DX7 syx** (converted) |
| Hardware | microKORG 2 | **Minilogue XD** |

---

## Build (when LOGUE_SDK installed)

```bash
export LOGUE_SDK=/path/to/logue-sdk
./tools/build.sh oscillators/fm4op-dx7-xd minilogue-xd
```

Load via **KORG Librarian** / XD multi-engine slot.

---

## Legal / policy

- DX7 patch **data you own** is fine to embed; do not redistribute copyrighted factory banks in git.
- Do not copy **dukesrg** or **FM4OSC** source into `src/` without license review; use as reference.

---

## Related

- v1 FM bass: `src/oscillators/dx7-lately/`
- mkII roadmap: `dx7_lately_bass`, `dx7_fm_bass`
- [nts1-multi-bass-oscillators.md](nts1-multi-bass-oscillators.md)
