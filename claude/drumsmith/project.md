# project.md — DRUMSMITH (Drum Sample & Kit Product Designer + DSP Audio Engineer Agent)

**Updated:** 2026-11-17 · **Completion:** Agent package 100% (in-repo) · **Workbench v1 published** (private page + docked agent) · Cross-check JS↔Python on 8 WAVs · UI/layout **not browser-smoke-tested** · First live kit run **started** (Tail Light Static W1 + INTAKE; no real renders verified yet)

## Scope

A four-layer agent (template format) that does two jobs in one: (1) designs drum kit products (concept, hit list, naming, folder and device-export spec, channel notes, QA gate) and (2) engineers the DSP behind the sounds (voice recipes, real-time-safe render rules, measurement and verification). Single agent plus one read-only QA subagent (CALIPER). Feeds Disklordz kits, Tail Light Static kit, SEANCE BOX sample library, and hands plugin work to FORGE. A web workbench UI with the agent docked beside it fronts the same rules (artifact; not committed in this repo).

**Monorepo path:** `claude/drumsmith/` · Claude Code symlinks: `.claude/agents/drumsmith.md`, `.claude/agents/caliper.md`, `.claude/skills/drumsmith-*`

## Proposed outcome & who it's for

Sam. Faster, verifiable drum kit production: every kit ships with a passing check, not a listening opinion.

## Done

- [x] Context read: agent-template.jsx, FORGE project.md, drum-engine-dsp skill, Disklordz / Seance Box / DPCS memory
- [x] L1 SOUL, L2 DUTIES (tiers, HITL gates, hard halts, known gaps), L3 system-prompt (5 workflows), L4 core-config (delivery profile)
- [x] agent.yaml, README (ADR), INTAKE (13 questions), CLAUDE.md
- [x] Subagent CALIPER (QA, read-only)
- [x] Skills: drum-voice-recipes, kit-product-spec, sample-delivery-qa
- [x] scripts: dsp_math.py, verify_kit.py, self_test.py — 19/19 tests pass, exit 0 (hand-derived expected values)
- [x] Tarball delivered: drumsmith.tar.gz (original chat deliverable)
- [x] **Land in Instruments monorepo** — PR branch `cursor/drumsmith-tail-light-static-58c6`, CI `drumsmith.yml`
- [x] Tail Light Static: **INTAKE filled** + **W1** `kits/tail-light-static/kit-spec.md` (ship status NOT READY)
- [x] Provenance stub: `docs/provenance/tail-light-static.md`
- [x] Workbench artifact (UI + AI agent) published, 2026-11-17: Kit spec builder (W1, open-question gate, folder tree, device status), Voice recipes (W2, parameter tables with S/C/A tokens, Web Audio preview for cowbell/808/kick), DSP calculators (6 cards, dsp_math port), Verify WAVs (W3, in-browser, files never uploaded), docked DRUMSMITH chat with 5 calculator tools, HITL Confirm/Abort buttons, spec save/copy
- [x] Verified: in-browser checker vs scripts/verify_kit.py on 8 WAVs — 0 per-check differences, same summary (7 fail, 4 warn); loop and glide values match hand-derived numbers (test harness `eval` name clash fixed before publish)

### Workbench v1 (published page)

**Link:** [DRUMSMITH Workbench](https://claude.ai/artifact/FwE8VaPi2a5vXRMPoX19zH) (private to Sam) · capabilities: **ask Claude** (page tools), **save/download**

| Tab | What it does |
|-----|----------------|
| **Kit spec** | Name, BPM range, hit counts, loops, devices → BLOCKED/READY banner, open questions, folder tree, promise-to-check table. Preload: Tail Light Static example (BPM/hit counts partly placeholder). |
| **Voice recipes** | Cowbell, 808, kick, drums, bus — S/C/A parameter tables; **Preview** (Web Audio sketch) on cowbell/808/kick — not the render core. |
| **DSP calculators** | Six cards — same formulas as `dsp_math.py` (loop length, glide sixteenths, cents error, …). |
| **Verify WAVs** | Local drop — **never uploaded**; thresholds match `verify_kit.py`. Three synthetic demos on open (PASS/FAIL examples). |
| **Agent** | Same rules as package; reads live spec + verify; calculator tools; **Confirm / Abort** on ship-level HITL. |

**First use:** Claude access prompt on first agent message; runs on viewer’s Claude usage. **Spec and verify state do not persist** between visits.

**Tested vs not tested (publish pass):** WAV checker parity with Python (**tested**, 8 files). Layout, audio preview, and chat panel (**not opened in browser** before publish).

## To do

- [x] Sam: copy agent folder to a repo root so CLAUDE.md and .claude/ auto-load (done via `claude/drumsmith/` + symlinks)
- [ ] First live run: **W3 verify on real renders** for Tail Light Static (INTAKE + W1 done in repo)
- [ ] Sam: **open workbench**, allow Claude access on first agent message, try quick prompts; **drop real Tail Light Static render** in Verify tab and report drift vs Python CLI
- [ ] Confirm device profiles (MPC 500, MPC Sample, Maschine+, MV-1, Lofi12, FL Studio) from sources or disklordz_slicer.py; all UNVERIFIED now
- [ ] Decide when to turn on evaluator_pass_enabled (CALIPER) in core-config.xml
- [ ] Add FFT/YIN pitch cross-check (numpy) for 808/cowbell key checks; add pitch check to the workbench Verify tab
- [ ] Add LUFS / certified true-peak metering (ffmpeg ebur128) to replace the approximate meter
- [ ] Expand provenance on first render batch (`docs/provenance/<kit>.md`)
- [ ] Workbench v2 ideas: persist spec between visits (db capability), share with collaborators, export per-device package list
- [ ] Optional: run verify_kit.py and the workbench through full DPCS phases if wanted
- [ ] Optional: commit workbench single-file HTML to repo (currently artifact-only)

## AI agents created

DRUMSMITH (orchestrator), CALIPER (QA subagent); workbench-embedded DRUMSMITH (artifact chat, same rules)

## Stack

Markdown/YAML/XML agent package, Claude Code (.claude/agents, .claude/skills), Python 3 stdlib only for scripts. Workbench: single-file HTML/CSS/JS artifact with the sample (ask Claude, page tools) and downloads capabilities, no libraries. Downstream: JUCE 8 / C++20 via FORGE.

## Artifact links

- Workbench UI + agent: https://claude.ai/artifact/FwE8VaPi2a5vXRMPoX19zH (private to Sam)
- Agent package in git: `claude/drumsmith/` (merged from **drumsmith.tar.gz**, 2026-11-17)

## Alternative uses / re-niching

Sellable "Drum Kit Product Agent" blueprint/prompt pack with the workbench as the demo UI (digital product direction); same pattern for melodic sample libraries (Sample Factory) or other audio product niches.

## API keys needed

None. The artifact's agent runs on the viewer's own Claude usage (asks consent on first message). Claude Code access to run the package.

## Opportunity value

Indirect. Supports Disklordz kit sales (Iron Cowbell pack plan priced $20–35, Sam's number) and the SEANCE BOX sample library. No forecast made here.

## Urgency

Medium. Tail Light Static kit is the first live use.

## Known gaps

Cannot listen; phonk numeric recipes are unsourced presets [A]; device profiles UNVERIFIED; true-peak meter approximate; pitch check is zero-crossing (sub-bass only) and not in the workbench yet; workbench state is not saved between visits; the browser preview synth is a sketch, not the render core; the agent panel needs Claude access in the viewing app.

## Build note

Agent-config package (same method as FORGE). The Python scripts and the workbench are small utilities verified by scripts/self_test.py (DONE = exit 0) and the JS-vs-Python cross-check; not run through the full DPCS phases.

## Quick verify (repo)

```bash
python3 claude/drumsmith/scripts/self_test.py
python3 claude/drumsmith/scripts/verify_kit.py <kit_dir> --bpm 74 --json
```