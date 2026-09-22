---
name: instrument-orchestrator
description: "Route Instrument & Sound Factory work across generation, maps, presets, JUCE CI, and HISE handoff. Use for WO intake, batch jobs, and choosing worker vs interactive mode."
---

# Instrument orchestrator

## When to use

- Multisample instruments, drum/product factory batches, preset packs, SFZ/MPC maps, or JUCE DSP tied to sound SKUs.
- User asks for **automated worker** or **batch creation**.

## Read first

1. [docs/INSTRUMENT_SOUND_FACTORY_AGENT.md](../../../docs/INSTRUMENT_SOUND_FACTORY_AGENT.md)
2. Product area: [ARCHITECTURE.md](../../../ARCHITECTURE.md) → JD / Wave909 / `disklordz/website` / `hise-sketch`
3. [AGENTS.md](../../../AGENTS.md) for test commands

## Runtimes

| Runtime | Use |
|---------|-----|
| **Cursor Cloud** | JUCE CI, Python/Node batch, SFZ, PRs |
| **VS Code (local)** | MPCTK `.xpj`, refs on disk, MPC hardware |
| **Antigravity** | HISE export; MPCTK on same Windows/Mac box |

Same skills and `disklordz/sound-factory/jobs/*.json`. Set `"runtimeHint"` on jobs when steps split across machines.

## Mode selection

| Mode | Behavior |
|------|----------|
| **worker** (default) | Execute job DAG; draft PR or handoff artifact; no chat unless blocked |
| **interactive** | Confirm MPC project writes, deploy, Zapier/Airtable writes |

**Worker trigger:** you start manually **or** PM Agent / Airtable WO → job JSON (skill `pm-airtable-factory-trigger`).

## Route to skills

| Task | Skill |
|------|--------|
| WAV batch / manifests | `audio-generation-batch` |
| SFZ / zones | `sfz-multisample-map` |
| MPC Keygroup programs | `mpc-keygroup-program` + **`cursor-mpc-agent`** |
| WO / PM kickoff | `pm-airtable-factory-trigger` |
| JD / Wave909 / HISE presets | `preset-design-factory` |
| Reference timbre (YouTube) | `sound-design-reference-match` |
| C++ DSP | `juce-dsp-plugin-ci` |
| HISE on Windows | `hise-antigravity-handoff` |

## Subagents

- `explore` — codebase discovery
- `debug` — non-obvious audio bugs
- `ci-investigator` — plugin CI red
- `best-of-n-runner` — parallel variation attempts
- `videoReview` — user-supplied reference video
- `computerUse` / Bytebot — MPC Software UI only when user expects desktop automation

## Definition of done

- Manifest + SHA-256 provenance for every WAV
- QA gates passed (see enterprise doc §9)
- Walkthrough evidence (spectrogram, CI log, or demo video)
- Draft PR; never merge or production deploy without user request
