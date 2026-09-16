# Instruments — Audio Plugin Coder (APC) agent guidance

Compatible with the **Noizefield APC** workflow
([audio-plugin-coder](https://github.com/Noizefield/audio-plugin-coder)) as shown in the
walkthrough series (Setup → Dream → Plan → Design → Implement → Test → Ship).

Canonical APC files in **this** repo:

| Area | Path |
|------|------|
| Workflows (slash commands) | `.agents/workflows/apc-*.md` |
| Phase skills | `.agents/skills/` |
| Rules | `.agents/rules/` |
| DSP architecture (extended) | `docs/dsp-architecture-specification.md` |
| User guide | `docs/apc-workflow.md` |

## First run

1. Run **`/apc-setup`** (or `bash bin/apc setup`).
2. Creates `apc.config.json` from `apc.config.example.json`.
3. Then **`/apc-dream MyPlugin`**.

## Mandatory VST lifecycle

Every VST/VSTi: **Dream → Plan → Design → Implement → Test → Ship**.  
See `docs/vst-lifecycle.md` and `.cursor/rules/10-vst-lifecycle-mandatory.mdc`.  
Ship = `tools/package-vst.sh` (not build-only).

## Slash commands (Cursor / any agent)

| Command | Phase |
|---------|--------|
| `/apc-setup` | Toolchain, paths, JUCE submodule |
| `/apc-dream [Name]` | Creative brief + parameter spec |
| `/apc-plan [Name]` | Architecture + complexity + UI path |
| `/apc-design [Name]` | GUI spec / mockup notes |
| `/apc-impl [Name]` | JUCE code under `vst/[Name]/` |
| `/apc-test [Name]` | Build + smoke validation |
| `/apc-debug [Name]` | Fix build/runtime issues |
| `/apc-ship [Name]` | Package to `release/` |
| `/apc-status [Name]` | Read `status.json` |
| `/apc-resume [Name]` | Continue last incomplete phase |
| `/apc-new [Name]` | Full pipeline with confirmations |

Short aliases (`/dream`, `/plan`, …) mean the same as `/apc-dream`, etc.

## Required context before editing a plugin

1. Read `plugins/<Name>/status.json`.
2. Resolve paths via `source scripts/lib/apc-paths.sh && apc_load_paths && apc_plugin_path <Name>`.
3. Load the workflow file for the current phase under `.agents/workflows/`.
4. Execute the matching skill under `.agents/skills/`.

## Phase gates

- Complete **only** the requested phase; update `status.json` when done.
- Do not start the next phase unless the user asks.
- If `setup.completed` is false, warn once and suggest `/apc-setup`.

## Build (this repo)

- Prefer: `bash bin/apc build <Name>` or `./tools/build-vst.sh <Name>` when present.
- JUCE lives at `vst/JUCE` (submodule).
- Implementation output: `vst/<Name>/` (created in **impl** phase via `tools/new-vst.sh`).

## Attribution

Do not add AI co-author trailers or list AI as project contributors unless the user asks.
