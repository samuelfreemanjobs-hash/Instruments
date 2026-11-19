# Scheduled GitHub automation (Instruments)

Sam’s repo uses **scheduled workflows** for queue ticks, QA, and catalog sync. None of these merge code by themselves except where a workflow explicitly commits to a **PR branch** (agent workflows) or `state.json` (autonomous tick).

## Multi-agent router (depends on PR #152)

Every 6 hours, `scripts/autonomous-coder/tick.py --emit-slice` then `--launch-agent` **classifies** the next plugin PR job:

| Tier | Examples | Routed to |
|------|-----------|-----------|
| **Easy** | Merge conflicts, failing CI/lint, workflow YAML, docs, small test fixes | **Gemini CLI** → **Aider** → **Jules issue** (`jules` label) → **Cursor** only if no free agent is configured |
| **Hard** | New DSP/JUCE, GUI, finishing a plugin to MVP | **Cursor Cloud** (`CURSOR_API_KEY`) |

Assignments are recorded in `disklordz/autonomous-coder/state.json` under `assigned_agents` (one agent per PR / slice).

## Agent setup table

| Agent | Handles | Secret / setup | Get it | Manual trigger |
|-------|---------|----------------|--------|----------------|
| **Cursor Cloud** | Hard plugin slices, MVP DSP/GUI | `CURSOR_API_KEY` | Cursor → **Integrations** → **API keys** | Scheduled tick or `tick.py --launch-agent` locally with key |
| **Gemini CLI** | Easy PR fixes in Actions | `GEMINI_API_KEY` | [Google AI Studio API keys](https://aistudio.google.com/apikey) | Actions → **Agent — Gemini CLI** (`workflow_dispatch`: `pr_number`, `task`) or PR label `agent:gemini` |
| **Aider** | Easy PR fixes via OpenRouter | `OPENROUTER_API_KEY` | [OpenRouter keys](https://openrouter.ai/keys) | **Agent — Aider** dispatch or label `agent:aider` |
| **Aider model (optional)** | Default free coding model | Repo variable `AIDER_MODEL` | [Free models on OpenRouter](https://openrouter.ai/collections/free-models) — default `openrouter/particle-ai/particle-1:free` | Set under Settings → Variables |
| **Jules** | Easy jobs via issue + label | Jules **GitHub App** on repo + optional `JULES_API_KEY` | Connect at [jules.google.com](https://jules.google.com/) · API key at [Jules Settings → API](https://jules.google.com/settings#api) | Autonomous tick creates/updates an issue with label **`jules`** ([docs](https://jules.google/docs/running-tasks/)) |
| **Cline (laptop)** | Sam’s Windows VS Code sessions | Cline extension + `.clinerules/` | [Install Cline](https://cline.bot/install) · rules in repo root `.clinerules/disklordz-instruments.md` | Open folder in VS Code; rules load automatically |
| **Cline CLI (optional CI)** | Same as easy jobs in Actions | `OPENROUTER_API_KEY` and/or `CLINE_API_KEY` | OpenRouter key or [Cline auth docs](https://docs.cline.bot/getting-started/authorizing-with-cline) | **Agent — Cline CLI (optional)** or label `agent:cline` |

Shared rules: `disklordz/autonomous-coder/agent-rules.md`, `GEMINI.md`, `AGENTS.md`.

### Fork safety

Agent workflows use `pull_request` **`labeled`** events and require `head.repo.full_name == github.repository`, or **`workflow_dispatch`** from maintainers — fork PRs do not receive secrets.

## Required secret for Cloud agent launches

| Secret | Purpose |
|--------|---------|
| `CURSOR_API_KEY` | Cursor dashboard → **Integrations** → **API keys** — `POST` Cursor Cloud Agents for **hard** jobs. |

If `CURSOR_API_KEY` is missing, hard jobs log a skip; easy jobs use free agents when configured.

## Scheduled workflows

| Workflow file | Schedule | What it does |
|---------------|----------|----------------|
| `autonomous-coding-agent.yml` | Every 6h (`:15`) | Antigravity sync, coach progress log, `NEXT_SLICE.md`, **routes** to Gemini/Aider/Jules/Cursor, commits `state.json`. |
| `agent-gemini.yml` | On demand / label | `google-github-actions/run-gemini-cli@v0.1.22` on PR branch. |
| `agent-aider.yml` | On demand / label | Headless `aider --message-file` + OpenRouter. |
| `agent-cline.yml` | On demand / label | Optional Cline CLI headless (BYOK). |
| `nightly-qa.yml` | Daily 11:00 UTC | `vst-testing-ops/run_business.py --profile ci`. |
| `black-friday-madness-go-live.yml` | 06:00 & 18:00 UTC | BFM catalog sync when Supabase secrets exist. |
| `disklordz-audio-windows.yml` | (see file) | Windows x64 CHORD ENGINE VST3 artifact. |
| `agent-fleet-execute.yml` | Weekdays | Fleet scaffold checks. |

`workflow_dispatch` is available on agent and tick workflows for manual reruns.

## Plugin priority queue

Plugin work is defined in `disklordz/autonomous-coder/plugin-pr-queue.json`. Closed or merged PRs are skipped via the GitHub API.

## Related

- [AUTONOMOUS_CODING_AGENT.md](AUTONOMOUS_CODING_AGENT.md)
- [disklordz/autonomous-coder/ARCHITECTURE.md](../disklordz/autonomous-coder/ARCHITECTURE.md)