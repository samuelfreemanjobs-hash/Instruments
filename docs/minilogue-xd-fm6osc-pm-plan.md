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

**manifest.json.example:**

```json
{
  "banks": [
    { "file": "bank_01.syx", "label": "Bass favorites" },
    { "file": "bank_02.syx", "label": "Keys / EP" }
  ]
}
```

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
