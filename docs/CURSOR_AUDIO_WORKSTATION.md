# Cursor audio workstation

How this repo turns Cursor into the VST and drum-kit workbench. Components live in `.cursor/` so a cloud agent and a laptop load the same files. Personal `~/.cursor/skills` is for experiments that should not ship with the repo.

## What loads where

| Feature | Path | Job |
|---------|------|-----|
| Constitution | `.cursor/rules/audio-business-constitution.mdc` | Always-on mode switch and pointers |
| Rules | `.cursor/rules/*.mdc` | DSP, UI, WebView, samples, licensing, IP, release |
| Skills | `.cursor/skills/*/SKILL.md` | `/drum-kit-factory`, `/drum-synth-dsp`, `/release-package`, and the rest |
| Commands | `.cursor/commands/*.md` | `/new-voice`, `/prep-kit`, `/bump-version`, `/hotfix`, `/weekly-ops` |
| Subagents | `.cursor/agents/*.md` | `dsp-auditor`, `dsp-reviewer`, `preset-designer`, `build-runner`, `ip-guard`, `release-engineer` |
| Hooks | `.cursor/hooks.json` | Format drum-engine C++, block destructive shell, flag secrets |
| MCP | `.cursor/mcp.json` | Supabase, Stripe, Context7, Playwright |
| Editor | `.vscode/extensions.json` | clangd, CMake Tools, Hex Editor, WAV Preview |

## MCP

| Server | How it connects | Do not |
|--------|-----------------|--------|
| Stripe | `https://mcp.stripe.com` OAuth, already in `.cursor/mcp.json` | Put `sk_live_` in git. Writes need a confirmation. |
| Supabase | Project ref from `SUPABASE_PROJECT_REF` | Commit the service role key. |
| Context7 | `https://mcp.context7.com/mcp/oauth` | Commit an API key. Connect in Cursor MCP settings. |
| Playwright | `npx @playwright/mcp@latest` | Use it for the storefront and preview players. |
| GitHub | Built-in Cursor GitHub connection (`cursor-github`) | Add a second GitHub MCP that needs a personal token. |

Linear, S3, and Shopify stay unwired until an env var exists. A server entry with an empty secret fails every session.

## CLI the agent can run

| Tool | Command |
|------|---------|
| Drum tests | `ctest --test-dir disklordz/drum-engine/build --output-on-failure` |
| One-shots | `DrumBatchExporter --out /tmp/dlz-kit --count 50 --voice all` |
| Kit zip | `python3 disklordz/drum-engine/python/process_kit.py RAW --out OUT --zip` |
| pluginval | `python3 scripts/vst/run_pluginval.py PATH.vst3` |
| Preview | `ffmpeg` via `stitch_preview.py --mp3`; spectrogram via `sox` |
| Notarize | `packaging/macos/notarize.sh` on a Mac. Exit 2 here. |

## Optional community skills

Review the license, then install locally. They are not vendored.

- [JUCE Agent Toolkit](https://github.com/danielraffel/juce-agent-toolkit) (MIT): `npx skills add https://github.com/danielraffel/juce-agent-toolkit --skill '*'`
- [TAP project planner](https://github.com/TheAudioProgrammer/claude-audio-plugin-project-planner-skill): interview-style PRD before C++. This repo still requires APC Dream/Plan/Design before a new `vst/<Name>/`.

## Extensions

Install from the recommendations in `.vscode/extensions.json`: clangd, CMake Tools, Hex Editor, WAV Preview. Point clangd at `disklordz/drum-engine/build/compile_commands.json` for this target. The repo `.clangd` file stays on `MyFirstPlugin/build`.

## Hooks

`beforeShellExecution` denies `rm -rf` of `sound-factory/kits` and a force-push onto main. `afterFileEdit` runs clang-format only under `disklordz/drum-engine` so JD Upgraded and the other plugins keep their current formatting. `stop` follows up once if the secret scan wrote `.cursor/hooks/state/violation.json`.
