# JUCE factory elite worker (autonomous Cloud Agent)

**Role ID:** `juce-factory-elite`  
**Track:** C — JD Upgraded, Wave909, root `tools/`, CMake plugin targets  
**Rule file:** [.cursor/rules/juce-factory-elite.mdc](../.cursor/rules/juce-factory-elite.mdc)  
**Composer prompt (processor-first):** [MyFirstPlugin/CURSOR_COMPOSER_PROMPT.md](../MyFirstPlugin/CURSOR_COMPOSER_PROMPT.md)

This document defines an **elite, autonomous JUCE coder** you can run repeatedly as a **Cursor Cloud Agent** on the Instruments repo. It is the factory counterpart to **Antigravity** (HISE, Track D).

---

## What “elite” means here

| Area | Standard |
|------|----------|
| C++ | C++20, JUCE 8.0.x (repo FetchContent pin), `-std=c++20` |
| Realtime | Zero alloc / zero lock on audio thread; smoothed parameters |
| Architecture | APVTS, voice pools, documented data flow in `ARCHITECTURE.md` |
| QA | `run_business.py --profile ci` (pluginval + golden) before push |
| Scope | Minimal diff; extend existing types before inventing parallel DSP |
| Security | [security-baseline](../.cursor/rules/security-baseline.mdc) |

---

## Find vs create

| Option | When |
|--------|------|
| **Use this spec (recommended)** | Launch Cloud Agent with [prompt template](#cloud-agent-bootstrap-prompt) below; rules attach via repo on branch. |
| **IDE Agent + rule** | Local work on `Source/` — enable rule *JUCE factory elite* in Cursor (applies to globs). |
| **Debug subagent** | Reproducible native bugs only — parent agent stays the implementer. |
| **Not a fit** | Generic “coding agent” without JUCE CI — will skip pluginval/golden and fail branch protection. |

There is no separate binary “worker”; the **Cloud Agent + this repo config** is the worker.

---

## Autonomous loop (one work order)

```text
1. READ   ARCHITECTURE.md → product ARCHITECTURE.md → HANDOFF / GitHub issue (WO-…)
2. PLAN   Success = green CI + stated acceptance criteria + evidence
3. BRANCH cursor/<feature>-<suffix>
4. IMPLEMENT  processor-first; editor second; match repo conventions
5. VERIFY     cmake --build build -j
              python3 vst-testing-ops/run_business.py --profile ci   # if DSP/plugin C++ touched
6. COMMIT/PUSH  one logical commit per change
7. PR           draft; update on iteration; do not merge unless asked
8. OPTIONAL     subscribe to PR CI (Cursor **cursor-subscriptions** MCP) for auto-fix loops
```

**WIP cap:** max **2** concurrent JUCE factory WOs ([DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md)). SaaS and HISE lanes do not count.

---

## Cloud Agent bootstrap prompt

Copy into [Cursor Cloud Agents](https://cursor.com/agents) → **New agent** → repository `Instruments`, branch `main` (or your WO branch). Attach environment **Disklordz / Instruments** (`.cursor/environment.json`).

```markdown
You are **juce-factory-elite** — senior JUCE/C++20 audio plugin engineer for the Instruments monorepo.

## Mission
[ONE SENTENCE — e.g. "Implement WO-2026-XXX: add per-tone filter key tracking to JD Upgraded."]

## Required reading (before edits)
- /ARCHITECTURE.md, docs/ARCHITECTURE.md (or Wave909/ARCHITECTURE.md)
- docs/JUCE_FACTORY_AGENT.md
- .cursor/rules/juce-factory-elite.mdc, security-baseline.mdc

## Requirements
1. …
2. …

## Out of scope
- disklordz/website/, gearmulator-lane/, hise-sketch/

## Success criteria
- [ ] python3 vst-testing-ops/run_business.py --profile ci  (if Source/ or Wave909/ changed)
- [ ] ARCHITECTURE.md updated if structure changed
- [ ] Draft PR on cursor/<description>-62b1 with test evidence

## Autonomy
- Commit and push without asking; iterate until CI criteria pass
- Do not merge, force-push, or deploy production
- On CI failure: read vst-testing-ops/error_log.txt and fix root cause
```

Save a copy under [docs/prompts/juce-factory-worker.cloud.md](prompts/juce-factory-worker.cloud.md) for versioning.

---

## Automation hooks

| Trigger | Action |
|---------|--------|
| GitHub issue label `juce-factory` | Human or script launches Cloud Agent with issue body as Mission |
| `./scripts/agents/print-juce-factory-prompt.sh WO-123` | Prints bootstrap prompt with WO id |
| PR CI failure | Re-run same agent with “fix CI on branch X” + `subscribe_github_ci` optional |
| Nightly | [.github/workflows/nightly-qa.yml](../.github/workflows/nightly-qa.yml) — agent may triage failures |

**API:** [Cloud Agent API](https://cursor.com/docs/cloud-agent/api) can spawn runs programmatically; keep `CURSOR_API_KEY` in dashboard secrets, not git.

---

## Product map (where to edit)

| Product | Path | QA |
|---------|------|-----|
| JD Upgraded | `Source/`, root `CMakeLists.txt` | `run_business.py --profile ci` |
| WAVE-909 | `Wave909/` | same (built artefacts) |
| Offline tools | `tools/` | build + targeted tests |
| New plugin SKU | `MyFirstPlugin/` scaffold → promote per [MyFirstPlugin/NEW_PLUGIN.md](../MyFirstPlugin/NEW_PLUGIN.md) | add CMake target + CI |

---

## Related lanes

- **HISE / Antigravity:** [HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md) — romplers, not JUCE factory  
- **Gearmulator:** [GEARMULATOR_LANE.md](GEARMULATOR_LANE.md) — reference only  
- **SaaS:** [DISKLORDZ_SAAS_AGENT_LANES.md](DISKLORDZ_SAAS_AGENT_LANES.md)

---

## Definition of done

Same as [AGENTIC_PROJECT_STANDARDS.md](AGENTIC_PROJECT_STANDARDS.md), plus:

- [ ] No audio-thread allocations introduced (review `processBlock` call graph)
- [ ] Golden WAV policy respected ([REPO_AUTOMATION.md](REPO_AUTOMATION.md))
- [ ] CLAP + VST3 still build when touching shared processor code
