# 24/7 autonomous Cloud Agent (Cursor)

This repo is set up for **Cursor Cloud Agents** and **Automations**: scheduled or event-driven runs that implement work on `main` without you starting each session manually.

## What is already in the repo

| Asset | Purpose |
|-------|---------|
| [`.cursor/environment.json`](../.cursor/environment.json) | Linux deps, full CMake build, golden verify on agent boot (overrides personal dashboard env when present on the branch) |
| [`.cursor/rules/autonomous-agent.mdc`](../.cursor/rules/autonomous-agent.mdc) | Default playbook: branch naming, tests, backlog, no merge without explicit user request |
| [`.github/workflows/build.yml`](../.github/workflows/build.yml) | CI on `main` and `cursor/**` |
| [docs/HANDOFF.md](HANDOFF.md) · [docs/PHASE5.md](PHASE5.md) | Current scope and follow-on work |

After you merge changes to `environment.json`, trigger a **environment build** in the dashboard so new agents boot from a warm snapshot (faster than re-fetching JUCE every run).

## Enable a 24/7 Automation (you do this in Cursor)

1. Open **[Cursor Dashboard → Cloud Agents → Automations](https://cursor.com/dashboard?tab=automations)** (or **Agents → Automations** in the app).
2. **New automation**
   - **Repository:** `samuelfreemanjobs-hash/Instruments`
   - **Base branch:** `main`
   - **Environment:** select **Instruments — JD Upgraded (Ubuntu, g++-12)** after the repo `environment.json` is on `main` and a build has succeeded (or use **Edit environment** linked to this repo).
3. **Trigger** (choose one or combine):
   - **Schedule** — e.g. every 6 hours or nightly cron for continuous backlog draining.
   - **GitHub** — on push to `main`, on PR comments, or on failed CI (if your plan exposes those triggers).
4. **Instructions** — paste a standing prompt, for example:

   ```
   Follow .cursor/rules/autonomous-agent.mdc. Pick one backlog item from docs/PHASE5.md,
   implement on a cursor/* branch, run golden verify if DSP changed, push, and open a draft PR.
   Summarize what you did and what is next. Do not merge.
   ```

5. **Model / limits** — pick a capable model; set max spend or run cap if your team uses quotas.
6. **Save** and turn the automation **On**.

## Requirements

- **Cursor plan** with Cloud Agents / Automations enabled (team or individual per your contract).
- **GitHub** access for Cursor to clone and push (install the Cursor GitHub app on `Instruments` if not already).
- Optional **secrets** in the environment dashboard for anything beyond public `FetchContent` (none required today).

## Faster boots (recommended)

1. Dashboard → **Cloud Agents → Environments** → open the environment for this repo.
2. Run **Build environment** after `environment.json` changes (validates `install` on a clean VM).
3. When the build is green, enable **Use build for new agents** (wording may vary) so installs are baked into the snapshot.

## What this is not

- A single long-lived chat in the IDE — each automation run is a **new** Cloud Agent session with your standing instructions.
- **Bugbot** — separate product; use Automations for proactive implementation work.
- **Self-hosted 24/7** — for always-on machines you control, see Cursor docs on self-hosted workers; this repo’s default path is Cursor-hosted Cloud Agents.

## Verify setup

1. Manually start one Cloud Agent on `main` with prompt: “Read docs/CLOUD_AGENT.md and confirm install + golden verify passed.”
2. Confirm setup logs show apt, cmake, and `All golden comparisons passed`.
3. Enable the Automation schedule.
