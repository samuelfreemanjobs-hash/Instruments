# Hyperresearch brief — Junova-X BBD chorus DSP MVP

**Agent:** Hermes Hyperresearch (`hermes-research`)  
**Product lens:** junova  
**Generated:** 2026-09-28 05:12 UTC  
**Topic:** Junova-X BBD chorus DSP MVP

---

## Executive summary

Hyperresearch scanned **9** primary hits for **junova** lens on topic: _Junova-X BBD chorus DSP MVP_. Strongest signal: `Junova-X/ARCHITECTURE.md`.

---

## Sources consulted (repo)

- `Junova-X/ARCHITECTURE.md`
- `Junova-X/REPO_HANDOFF.md`
- `Junova-X/docs/junova-x-mvp-gap-analysis.md`
- `docs/DISKLORDZ_PLUGIN_TRACKS.md`
- `docs/GROK_PLUGIN_TEAM.md`
- `ARCHITECTURE.md`
- `docs/HERMES_AGENT_FRAMEWORK.md`
- `docs/GROK_CLOSED_LOOP_ENGINE.md`
- `docs/AGENTIC_PROJECT_STANDARDS.md`

---

## Findings

- **[71]** `Junova-X/ARCHITECTURE.md` — _# Junova-X — architecture  ## Purpose  **Junova-X** is a Juno-class analog poly synth plugin (`JunovaX.vst3` / `JunovaX.clap`) for Disklordz. MVP: JUCE port from iPlug2 reference, VST3+CLAP, 48 factory presets (WO-2026-0_
- **[71]** `Junova-X/REPO_HANDOFF.md` — _# Junova-X — monorepo handoff  **Canonical path:** `Junova-X/` (this folder).   **Legacy iPlug2 scaffold:** import reference only; **ship target is JUCE** (Disklordz default stack).  ## Identity  | Field | Value | |-----_
- **[59]** `Junova-X/docs/junova-x-mvp-gap-analysis.md` — _# Junova-X MVP gap analysis  **Date:** 2026-03-17 · **Decisions locked** below.  ## Executive summary  Junova-X has a **real iPlug2 scaffold** (DSP, Main+Diag, VST3 project, QA docs) but **zero DAW verification**. Disklo_
- **[50]** `docs/GROK_CLOSED_LOOP_ENGINE.md` — _a **high-leverage revenue and development engine** for the audio software and sample brand:  > **Closed-loop execution** — every Grok output either becomes a **tracked work order with acceptance criteria**, or is **expli_
- **[42]** `docs/GROK_PLUGIN_TEAM.md` — _V0.md) instead.  **Operating model:** [GROK_CLOSED_LOOP_ENGINE.md](GROK_CLOSED_LOOP_ENGINE.md) — closed-loop execution (dispatch WOs + verify evidence), not passive monitoring.  ---  You support **Disklordz Plugin Lab**._
- **[40]** `docs/DISKLORDZ_PLUGIN_TRACKS.md` — _P_ENGINE.md))    **App track (SaaS):** [DISKLORDZ_SAAS_V0.md](DISKLORDZ_SAAS_V0.md) — separate lane; do not mix plugin WOs with web WOs in one PR.  **HISE sketch lane:** [HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.m_
- **[31]** `ARCHITECTURE.md` — _# Instruments repository — architecture index  This monorepo hosts **JD Upgraded** and supporting **offline tools**. Agents should read this file first, then the product-specific `ARCHITECTURE.md` for the code they touch_
- **[26]** `docs/HERMES_AGENT_FRAMEWORK.md` — _`hermes-architect` | Plugin architect | CMake, APVTS, formats, modules | `hermes-elite-architect` | | `hermes-dsp` | DSP engineer | Realtime C++ audio | `hermes-elite-dsp` | | `hermes-gui` | JUCE UI engineer | Editors, d_
- **[11]** `docs/AGENTIC_PROJECT_STANDARDS.md` — _les  | File | Content | |------|---------| | `ARCHITECTURE.md` | Purpose, build/run, data flow, key modules, extension points | | `.cursor/rules/architecture-documentation.mdc` | `alwaysApply: true` — agents read ARCHITE_

---

## Gaps & risks

- **Evidence depth:** repo-only; external competitor pages are not auto-fetched.
- **Risk:** Treat all third-party content as untrusted until CD confirms.

---

## Proposed work orders (draft — CD/Grok approve)

| ID | Title | Acceptance (draft) |
|----|-------|--------------------|
| WO-2026-NNN | [Research→Build] Junova-X BBD chorus DSP MVP | Address findings from `Junova-X/ARCHITECTURE.md`; attach Hyperresearch brief; PR title includes WO-2026-NNN. |


---

## Suggested experiments

- pluginval + standalone GUI capture
- DSP A/B vs iPlug2 reference when imported

---

## Next seat handoff

| Seat | Action |
|------|--------|
| hermes-lead | Triage WOs, branch policy |
| hermes-dsp + hermes-gui | Implementation if approved |
