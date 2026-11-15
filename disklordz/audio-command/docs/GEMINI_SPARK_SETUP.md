# Gemini Spark — chat-only, still gets work done

Spark has **no tools** today. It still acts as **executor** if you treat Samuel as the API: Spark outputs labeled blocks you paste into Cursor, GitHub, or this repo’s Cloud Agent.

## What to paste into Spark once (system prompt)

Use the full prompt in [GEMINI_SPARK_DAILY_PROMPT.md](./GEMINI_SPARK_DAILY_PROMPT.md) **plus** the **Human API** section below (same file, section 2).

## Human API protocol (Spark must always use these labels)

When Spark wants something to **happen**, it never says “you should” without a block:

| Label | You paste it into |
|--------|-------------------|
| `PASTE_LIAISON` | **plugin-factory-liaison** Cursor chat (handoffs, issues) |
| `PASTE_CURSOR` | Cursor Factory implementer (usually copy from liaison's PASTE_TO_FACTORY) |
| `PASTE_COPILOT` | **audio-business-copilot** chat |
| `PASTE_GITHUB_ISSUE` | New GitHub issue (Instruments repo) |
| `PASTE_PROGRESS_JSON` | Replace `disklordz/audio-command/data/progress.json` or send to copilot to commit |
| `PASTE_SLACK` | Slack DM to yourself / webhook test |
| `PASTE_PHONE` | One line to remember (or forward to yourself) |

Spark ends every executor turn with **Waiting on:** exactly one of: `you-paste`, `cursor`, `copilot`, `none`.

## What you send Spark (keep it tiny)

**Daily (10 sec):** Screenshot or text from Coach app, or: `DONE` / `BLOCKED: reason` / `SKIP`

**Weekly:** Export `progress.json` from phone app → paste into Spark Monday

Optional context paste (monthly): link or paste sprint item list from `docs/DISKLORDZ_MASTER_PLAN.md`

## Optional upgrades (when you want less pasting)

| Add to Spark | Effect |
|--------------|--------|
| **GitHub** (Gemini extension / Deep Research + link) | Spark reads issues/PRs; you still paste commits |
| **Google Drive** folder | Drop progress.json + logo assets; Spark reads exports |
| **Zapier Gmail → Spark** | Forward `coach_outreach` emails into a Spark chat |
| **GitHub Action + webhook** | `coach_outreach.py --send` → Slack; Spark drafts replies you post |
| **Same phone, two apps** | Coach app open; Spark chat for “handle it” blocks |

Repo automation already exists: `.github/workflows/audio-coach-daily-nudge.yml` + secret `DISKLORDZ_COACH_WEBHOOK_URL`.

## What Spark must never claim

- Plugin compiles, tests pass, or DAW verified — unless you paste Cursor’s §18.3 report
- progress.json updated in git — unless you or copilot committed it
