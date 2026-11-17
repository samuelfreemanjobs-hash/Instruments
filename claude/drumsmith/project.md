# project.md — DRUMSMITH (Drum Sample & Kit Product Designer + DSP Audio Engineer Agent)

**Updated:** 2026-11-17 · **In-repo:** agent package (v1.0 tarball) + workbench HTML · **Announced (packages not yet on Cloud VM):** v1.1 → v1.2 → v1.3 · First live kit W1 done; commercial release still blocked

## Scope

A four-layer agent (template format) that does two jobs in one: (1) designs drum kit products (concept, hit list, naming, folder and device-export spec, channel notes, QA gate) and (2) engineers the DSP behind the sounds (voice recipes, real-time-safe render rules, measurement and verification). Single agent plus one read-only QA subagent (CALIPER). Feeds Disklordz kits, Tail Light Static kit, SEANCE BOX sample library, and hands plugin work to FORGE. A web workbench UI with the agent docked beside it fronts the same rules. **Template in git:** `workbench/DRUMSMITH_Workbench.html` (hosted artifact for docked agent + save).

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

## Announced upgrades (packages pending upload)

Cloud uploads today: `drumsmith.tar.gz` (v1.0) + `DRUMSMITH_Workbench.html` only. **v1.1 / v1.2 / v1.3 ZIP or TAR.GZ are not on this VM** — attach the latest package (prefer **v1.3**) to merge into `claude/drumsmith/` the same way as the v1.0 tarball.

| Version | Claimed (external build) | Expected entrypoints |
|---------|--------------------------|----------------------|
| **v1.1** | 28 tests · 64 WAVs · 8/8 808 pitch · scaffold/render/verify pipeline · release-gate SOP | `scaffold_kit.py`, `render_reference.py`, `verify_kit.py --manifest` |
| **v1.2** | 38 tests · recipe engine · SHA-256 fingerprints · diversity audit · FORGE JSON handoff · 64/64 tech PASS · 13 envelope-similar pairs (ear) | `build_recipe_kit.py`, `render_recipes.py`, `diversity_audit.py`, `export_forge.py`, `test_upgrade.py` |
| **v1.3** | 44 tests · 5 genre profiles · `factory.py` · `kit_browser.py` @ `:8765` · Drift Phonk TLS 64/64 | `factory.py --genre drift-phonk`, `kit_browser.py`, `test_v13.py` |

**Ship gates (unchanged):** technical PASS ≠ commercial release. Named ear approval + device profiles tested still required. Pitch methods are dependency-free approximations (not certified FFT/YIN). v1.2/v1.3 Python engines are **offline reference** — FORGE handoff is JSON, not a compiled VST3.

**v2.0 (roadmap only):** DiskLordz Drum Manufacturing System — genre engines in JUCE 8/C++20, variation, desktop UI, MPC export verification. Not started in this repo.

## To do

- [ ] **BLOCKER:** Upload `drumsmith-v1.3.zip` (or v1.2 if v1.3 unavailable) to Cloud so it can replace/merge under `claude/drumsmith/`
- [x] Sam: copy agent folder to a repo root so CLAUDE.md and .claude/ auto-load (done via `claude/drumsmith/` + symlinks)
- [x] Workbench template in repo: `workbench/DRUMSMITH_Workbench.html`
- [ ] Merge latest package; run advertised tests (`self_test` + `test_upgrade` / `test_v13`); store Tail Light Static kit under `kits/` or package `TLS_*`
- [ ] First live run: **W3 verify on real renders** for Tail Light Static
- [ ] Sam: **open workbench**, allow Claude access; drop TLS renders in Verify tab
- [ ] Ear review of envelope-similar pairs (v1.2 noted 13); named release authorization
- [ ] Confirm device profiles (MPC 500, MPC Sample, Maschine+, MV-1, Lofi12, FL Studio); all UNVERIFIED now
- [ ] Decide when to turn on evaluator_pass_enabled (CALIPER) in core-config.xml
- [ ] Add LUFS / certified true-peak metering (ffmpeg ebur128) for release-critical claims
- [ ] Expand provenance on first render batch (`docs/provenance/<kit>.md`)
- [ ] Workbench v2: persist spec, share, per-device export list
- [ ] Optional: full DPCS phases for verify/render scripts
- [ ] Optional: start v2.0 FORGE/JUCE drum factory (separate product lane)

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