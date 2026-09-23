# PM plan — FM6OSC (Minilogue XD) + DX7 patch library

**Product:** **FM6OSC** (`fm6op-dx7-xd`) — 6-operator FM custom oscillator for **Minilogue XD**.  
**Owner:** Personal / Instruments lane ( **not** mkII `src/mkii/` collection ).  
**Agent:** **Korg Logue Agent** (autonomous execution when unblocked).  
**Status:** **Planning — blocked on owner DX7 preset files** (no build/sign-off until banks supplied).

**Technical companion:** [minilogue-xd-fm4-dx7-oscillator-plan.md](minilogue-xd-fm4-dx7-oscillator-plan.md)  
**Code scaffold (v0, do not ship yet):** `src/oscillators/fm6op-dx7-xd/`

---

## What “embed banks” means (plain language)

A **DX7 bank** is a file (usually **`.syx`**) containing **32 voices** × **128 bytes** of parameter data each (**4096 bytes** of voice data per bank, plus a small SysEx header).

**Embed banks** means:

1. You (or a script) **convert** those `.syx` files on your **computer** into a **binary blob** our oscillator understands (`Voice6` records + optional index table).
2. That blob is **linked into the oscillator project** as **read-only data** (like a sample ROM).
3. When you run **`./tools/build.sh … minilogue-xd`**, the resulting **`.prg` / multi-engine file** already **contains** your patches — they live **inside the oscillator binary**.
4. On the **Minilogue XD**, you **load one custom oscillator** into the multi-engine slot. There is **no** separate “import patches from USB” step on the hardware for standard logue user oscs.
5. You **change patches** with the **Patch** knob/param (voice 1–32 within the embedded bank), or by loading a **different built oscillator** if you maintain multiple `.prg` variants (e.g. “FM6OSC Bank A”, “FM6OSC Bank B”).

**What embed banks is not:**

| Misconception | Reality |
|---------------|---------|
| XD reads `.syx` from SD card at runtime | **No** — not supported by stock logue user osc API |
| One `.prg` holds unlimited DX7 libraries | **No** — flash size caps how many banks/voices fit |
| Live SysEx dump from DX7 editor while playing | **Non-standard** — possible only with custom tooling/MIDI bridge (out of v1 scope) |
| Same as FM4OSC patch files on microKORG | **No** — FM4OSC uses Korg format; we use **Yamaha DX7** import |

**Analogy:** Embedding banks is like **burning a sample ROM into a synth chip** — the presets travel **with the oscillator file** you load in Librarian, not as separate files on the keyboard.

---

## PM work orders (autonomous queue)

Use **`WO-LOGUE-FM6-*`** in PR titles when implementation starts.

| WO ID | Title | Owner gate | Agent can start when |
|-------|--------|------------|----------------------|
| **WO-LOGUE-FM6-001** | Voice6 spec + DX7 128-byte field map | — | **Now** (no syx needed) |
| **WO-LOGUE-FM6-002** | `import_dx7_bank.py` → `Voice6` + `voices.bin` | — | **Now** (use public domain / single test bank in repo only) |
| **WO-LOGUE-FM6-003** | DX7 **32 algorithm** router (float sin engine) | — | After 001 |
| **WO-LOGUE-FM6-004** | Embed 1× bank in FM6OSC + Patch param 1–32 | **Owner provides ≥1 `.syx`** | Owner drops files in agreed path |
| **WO-LOGUE-FM6-005** | Multi-bank build (up to 4×32 voices) + flash budget doc | Owner syx list | 004 green |
| **WO-LOGUE-FM6-006** | Velocity via XD assignable controller + doc | — | Parallel with 004 |
| **WO-LOGUE-FM6-007** | CPU profile on XD (FX on/off, poly notes) | Hardware | Owner or lab |
| **WO-LOGUE-FM6-008** | `dev_id` / `prg_id` + release checklist | Owner | Pre-release |

**Current blocker:** **WO-LOGUE-FM6-004+** — owner **does not have FM presets available** → stay in **001–003** (spec + tooling + algo router) only until files arrive.

---

## WO-LOGUE-FM6-005 — multi-bank plan (128 voices in one `.prg`)

**Goal:** After **WO-004** proves **1× bank × 32 patches** on hardware, scale to **up to four embedded DX7 banks** (**128 voices**) in a **single** FM6OSC load — same UX pattern as dukesrg **FM64** (`FM64.sh` injects **4×4096-byte** raw banks).

### User-facing behavior (target)

| Control | WO-004 (v1) | WO-005 (scale) |
|---------|-------------|----------------|
| **Patch** | Voice **1–32** within the only bank | Voice **1–32** within the **selected bank** |
| **Bank** (new or repurposed param) | Fixed bank 0 | **Bank A–D** (0–3) → which embedded `.syx` was baked in |
| **Algo / Level / Velo / …** | Unchanged | Unchanged (per-voice still from embedded data) |

**Panel constraint:** logue manifest allows **6 params** in v1 header. WO-005 must pick one layout:

1. **Recommended:** Add **Bank** by **splitting Patch** into coarse/fine (e.g. Patch = 0–127 maps to bank×32+slot), **or** replace **Bright** with **Bank** on a “library” build flavor documented in README.
2. **Alternative:** Keep 6 params; **Bank** only via **separate `.prg` builds** (no UI bank switch) — see “Strategy B” below.

Document the chosen layout in `src/oscillators/fm6op-dx7-xd/README.md` when implementing.

### Build / data layout

```text
reference/dx7-banks/manifest.json     ← ordered list, max 4 entries for single .prg
        ↓
tools/dx7/import_dx7_bank.py --multi …   ← WO-005: concat banks + header
        ↓
src/oscillators/fm6op-dx7-xd/voices.bin
  [header: magic, num_banks, voice_count, Voice6 stride]
  [bank0: 32 × Voice6]
  [bank1: 32 × Voice6]
  …
        ↓
./tools/build.sh oscillators/fm6op-dx7-xd minilogue-xd
        ↓
FM6OSC.prg  (code + embedded voices.bin)
```

**Runtime lookup:** `voice_index = bank * 32 + (patch - 1)`, clamp to embedded count.

### Flash budget (must measure, do not guess)

| Component | Order of magnitude | Notes |
|-----------|-------------------|--------|
| Raw DX7 per bank | **4096 B** | 32×128 — lower bound if we stored raw sysex |
| **Voice6** per voice | TBD in **WO-001** | Likely **≤128 B** mapped; could be **smaller** than raw if we quantize |
| **4 banks** voice ROM | **~16 KiB–32 KiB** | Plus **~50–200 KiB** oscillator **code** (algo router, sin, EG) |
| logue **.prg** limit | Platform-specific | WO-005 deliverable: **`docs/minilogue-xd-fm6-flash-budget.md`** with **linker map** from one Release build |

**Acceptance:** Build succeeds with **4 owner banks** embedded; **Patch 1–32** audibly changes timbre on **each** bank; linker report archived in PM notes (no syx in git).

### Implementation strategies (pick one primary, support one fallback)

| Strategy | What you load on XD | When to use |
|----------|---------------------|-------------|
| **A — Multi-bank `.prg`** | One FM6OSC with **Bank + Patch** (up to **128** voices) | Default once flash check passes |
| **B — Bank SKUs** | Multiple builds: `FM6OSC-Bass.prg`, `FM6OSC-Keys.prg`, each **32** voices | Flash tight, or you want **simple** panel (Patch only) |
| **C — Hybrid** | One “daily driver” 4-bank `.prg` + occasional **SKU** builds for gig sets | Owner preference after A/B listen |

**Tooling:** `import_dx7_bank.py` gains `--manifest reference/dx7-banks/manifest.json --max-banks 4 --out voices.bin`. CI (optional) builds **SKU** matrix from `manifest.json` without committing syx.

### WO-005 subtasks (agent checklist)

| ID | Task | Done when |
|----|------|-----------|
| 005a | **`voices.bin` format v2** (header + N banks) | Parser + C `const` loader agree on layout |
| 005b | **Bank selection** in `fm6op_dx7.cc` | Switching bank changes patch set without reload |
| 005c | **`minilogue-xd-fm6-flash-budget.md`** | Table: code size, voice ROM, headroom vs 4 banks |
| 005d | **manifest.json** schema v2 (`max_banks`, per-voice labels optional) | Example committed; real manifest gitignored |
| 005e | **Rebuild recipe** in README | “Edit manifest → import → build → Librarian” |

**Depends on:** WO-004 green on hardware; owner provides **≥2 `.syx`** files to validate bank switching (can be duplicates in dev).

---

## What else is possible (roadmap beyond WO-005)

Not committed work orders — options if you want more later. Grouped by **effort vs payoff**.

### More presets / libraries (still embed-at-build)

| Idea | Rough capacity | Tradeoff |
|------|----------------|----------|
| **>4 banks in one `.prg`** | 5–8 banks if Voice6 compact + code diet | May exceed flash; slower Librarian loads |
| **“Best of” compiler** | 32 slots filled from **many** syx files | Tool picks N favorites; you curate list in manifest |
| **Per-gig `.prg` flavors** | Same engine, different `manifest` → many binaries | No new code; swap file in Librarian before show |
| **FM4 + FM6 split** | FM4OSC-style **4-op** `.prg` for DX21-ish / lighter CPU | [fm4op-dx7-xd](minilogue-xd-fm4-dx7-oscillator-plan.md) — same import pipeline, different mapper |

### Performance / fidelity

| Idea | Notes |
|------|--------|
| **Full DX7 32-algorithm router** | WO-003 scope; some algos may need **quality tiers** (document “unsupported / degraded”) |
| **Per-op feedback & detune** | Panel **Detune** / **Bright** as global modifiers on top of embedded voice |
| **Assignable velocity** | WO-006 — FM64 pattern (no true per-note vel to osc on logue) |
| **mkII `dx7_fm_bass` port** | Heavier CPU / poly on **NTS-1 mkII** lane — not XD, same syx toolchain |
| **Golden A/B vs Dexed** | Optional test bank (public domain) in repo for regression only |

### Workflow / tooling (PC-side, not on keyboard)

| Idea | Notes |
|------|--------|
| **Dexed → syx → import → build** one command | Shell wrapper over existing import + build |
| **Bank diff / patch rename** in manifest | OLED names unlikely on XD; names live in **companion doc** or DAW preset list |
| **Hash-verified rebuild** | manifest stores sha256 of each syx; import refuses drift |

### Generally **not** realistic on stock logue v1

| Idea | Why |
|------|-----|
| **Load `.syx` from XD USB/SD while playing** | No official user-osc API for patch RAM |
| **Unlimited live SysEx stream from editor** | Custom MIDI bridge + host app — separate product |
| **Bit-identical Yamaha DX7** | Different clock, DAC, and often **6-op approximations** on M4 |
| **microKORG FM4OSC patch files** | Wrong format — use DX7 syx only |

### Optional future work orders (if you promote an idea)

| WO ID | Title | Trigger |
|-------|--------|---------|
| **WO-LOGUE-FM6-009** | “Best-of-32” manifest compiler across many syx | Owner has large library, wants one performance bank |
| **WO-LOGUE-FM6-010** | FM6OSC **Bank SKU** matrix in CI | Strategy B becomes primary |
| **WO-LOGUE-FM6-011** | Shared import CLI + Dexed one-shot rebuild | Workflow polish after 005 |
| **WO-LOGUE-FM6-012** | mkII 6-op FM unit sharing Voice6 + voices.bin | CPU headroom on mkII |

---

## Owner deliverables (when ready)

Create folder (gitignored, local only):

```text
reference/dx7-banks/
  bank_01.syx
  bank_02.syx
  ...
  manifest.json   ← optional: human names per voice slot
```

**Do not commit** copyrighted factory banks to GitHub. Commit **only**:

- `manifest.json.example`
- Hashes/counts in PM notes
- Tool output format docs

**manifest.json.example** (WO-004: one bank; WO-005: up to **4** for multi-bank `.prg`):

```json
{
  "build_flavor": "multi",
  "max_banks": 4,
  "banks": [
    { "file": "bank_01.syx", "label": "Bass favorites" },
    { "file": "bank_02.syx", "label": "Keys / EP" },
    { "file": "bank_03.syx", "label": "Brass / strings" },
    { "file": "bank_04.syx", "label": "FX / bells" }
  ],
  "voice_labels": {
    "0:0": "Lately Bass",
    "1:5": "Full Tines"
  }
}
```

(`voice_labels` keys are `"bank_index:patch_index_0based"` — optional, for README / DAW notes only.)

---

## Autonomous agent phases (execution order)

```text
Phase A  (NOW — no syx)     WO-001 spec, WO-002 parser, WO-003 algo router skeleton
Phase B  (owner syx)        WO-004 embed 1 bank, verify Patch 1–32 on hardware
Phase C  (scale)            WO-005 multi-bank, WO-006 velocity doc, preset curves
Phase D  (ship)             WO-007 profile, WO-008 IDs + README for XD load
```

**Parallel lane (optional):** Keep **`fm4op-dx7-xd`** for lighter patches; **FM6OSC** is primary for full DX7.

**Reference (study only):** `reference/logue/logue-osc/FM64-osc` + `FM64.sh` — same *embed-at-build* pattern, different codebase.

---

## Success criteria (definition of done)

- [ ] Owner hears **≥8 recognizable** DX7 voices via **Patch** knob on XD (subjective A/B vs desktop DX7 or Dexed).
- [ ] **Algo** param changes routing without silence on supported algorithms.
- [ ] **Velo** documented and wired to assignable controller.
- [ ] Rebuild recipe documented: “new syx → run import → build → Librarian load”.
- [ ] No copyrighted `.syx` in git.

---

## Team / PM integration

| Touchpoint | Action |
|------------|--------|
| Weekly intake | Do **not** re-propose FM6OSC — already planned here |
| GitHub label | `logue-implement` + comment `slug: fm6op-dx7-xd` when owner unblocks syx |
| Slack | Notify when WO-004 completes (needs hardware listen) |
| RAG | This file + [logue-agent-pm-automation.md](logue-agent-pm-automation.md) |

---

## Related

- [logue-oscillator-ideas-backlog.md](logue-oscillator-ideas-backlog.md)
- [logue-external-reference-repos.md](logue-external-reference-repos.md)
- `tools/dx7/import_dx7_bank.py`
