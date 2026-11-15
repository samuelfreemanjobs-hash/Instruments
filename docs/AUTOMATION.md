# Scheduled GitHub automation (Instruments)

Sam’s repo uses **scheduled workflows** for queue ticks, QA, and catalog sync. None of these merge code by themselves except where a workflow explicitly commits state (autonomous-coder `state.json` only).

## Required secret for Cloud agent launches

| Secret | Purpose |
|--------|---------|
| `CURSOR_API_KEY` | Cursor dashboard → **Integrations** → **API keys** — used by `scripts/autonomous-coder/tick.py --launch-agent` to `POST` Cursor Cloud Agents (`https://api.cursor.com/v0/agents`, fallback `v1`). |

If `CURSOR_API_KEY` is missing, the autonomous tick **logs a skip** and exits 0 (no failure).

## Scheduled workflows

| Workflow file | Schedule | What it does |
|---------------|----------|----------------|
| `autonomous-coding-agent.yml` | Every 6h (`:15`) | Antigravity sync, coach progress log, writes `NEXT_SLICE.md`, **launches a Cursor Cloud agent** when a plugin PR slice is queued, commits `state.json`. |
| `nightly-qa.yml` | Daily 11:00 UTC | `vst-testing-ops/run_business.py --profile ci` (VST configure/build/pluginval parity). |
| `black-friday-madness-go-live.yml` | 06:00 & 18:00 UTC | BFM catalog sync + live smoke when Supabase secrets exist; **scheduled runs skip cleanly** when secrets are absent. |
| `disklordz-audio-windows.yml` | (see file) | Windows x64 CHORD ENGINE VST3 artifact for human DAW proof. |
| `agent-fleet-execute.yml` | Weekdays | Safe fleet scaffold checks via `disklordz/agents/workflows/fleet_execute.py`. |

`workflow_dispatch` is available on most workflows for manual reruns from the Actions tab.

## Plugin priority queue

Plugin work is defined in `disklordz/autonomous-coder/plugin-pr-queue.json` (Trap Lord #146 first, then Plugg VA #106, …). Closed or merged PRs are skipped automatically via the GitHub API.

## Related

- [AUTONOMOUS_CODING_AGENT.md](AUTONOMOUS_CODING_AGENT.md)
- [disklordz/autonomous-coder/ARCHITECTURE.md](../disklordz/autonomous-coder/ARCHITECTURE.md)