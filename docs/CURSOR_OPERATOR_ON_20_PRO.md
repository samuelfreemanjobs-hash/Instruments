# Cursor Pro ($20) — operator playbook (DiskLordz Audio)

**Audience:** Samuel — one human, many agents in this monorepo.  
**Goal:** Cheap, clear compute tiers; no bouncing between Cursor chats.

## Billing (check dashboard)

| Item | Notes |
|------|--------|
| **Pro subscription** | Floor cost; not “unlimited” Cloud Agents. |
| **Usage pool** | Cloud Agent runs consume included API/model usage; heavy models burn faster. |
| **On-demand** | Optional overage — set a **spend limit** at [cursor.com/dashboard](https://cursor.com/dashboard). |
| **My Machines** | Your PC’s power + time; agent still bills against Cursor usage. Feature gate may vary by plan — confirm in **Settings → Cloud Agents**. |
| **GitHub Actions** | Separate from Cursor; use for Windows VST3 artifacts. |

**[assumed]** Exact minutes and My Machines eligibility change — verify in-app monthly.

## Three compute tiers (pick one per task)

| Tier | Where | Use for |
|------|--------|---------|
| **A — GitHub** | `windows-latest` | VST3 artifacts; phone watches Actions (DiskLordz-Audio repo CI: not yet) |
| **B — My Machine** | `agent worker start` on `C:\dev\Instruments` | MSVC iterate, secrets on box |
| **C — Cloud Linux** | This agent / PR branch | Docs, JSON, workflows — **not** DAW proof |

DiskLordz Audio product code lives in the private repo `samuelfreemanjobs-hash/DiskLordz-Audio` (`C:\DiskLordz\DiskLordz-Audio`); local build: `Build\Windows\build.ps1 -Release` there.

## Does the PC stay on?

| Mode | PC on? |
|------|--------|
| My Machines worker | **Yes** (or always-on Windows VM) |
| Local build | Only during build |
| GitHub Actions | **No** — download artifact on a DAW machine |

## One inbox (avoid chat sprawl)

1. **Coach PWA** — single sprint task (`disklordz/audio-command/pwa/`).
2. **Slack #disklordz** — daily nudge, not the checklist.
3. **`progress.json`** — committed truth when you export / update.
4. **`PASTE_TO_FACTORY.md`** — Windows build instructions; Factory only if build fails.

Map: [AUDIO_PREP_HANDOFF_BUNDLE.md](AUDIO_PREP_HANDOFF_BUNDLE.md).

## OpenClaw / Discord / Antigravity

Cursor does **not** command an OpenClaw fleet. In this repo:

- **Antigravity** — Windows handoffs (`watch-handoffs.ps1`).
- **OpenClaw** — parallel artist lane ([DISKLORDZ_SAAS_AGENT_LANES.md](DISKLORDZ_SAAS_AGENT_LANES.md)).
- **Discord** — optional; mirror Slack pattern (webhook → status), not a second brain.

Cloud agent = **bounded slices** (liaison, CI, docs), not “run all subagents.”

## Self-hosted Windows worker (optional)

```powershell
irm 'https://cursor.com/install?win32=true' | iex
agent login
cd C:\dev\Instruments
agent worker start --name "samuel-windows-instruments" --verbose
```

Then at [cursor.com/agents](https://cursor.com/agents) → **My Machines**. Docs: [My Machines](https://cursor.com/docs/cloud-agent/my-machines).

## This week’s critical path

Audio copilot + Stage B merged on **`main`** (PR #112). Scheduled workflows (audio fleet, antigravity, autonomous tick, Windows VST3 CI) run from default branch.

Coach **`plugin-skeleton`**: `git pull origin main` → build or **Actions** artifact → REAPER/FL → mark done.  
Defer: tokens, product-tree drafts, new operator apps until after DAW proof.

## Related

- [AUDIO_PLUGIN_AGENT_LANE.md](AUDIO_PLUGIN_AGENT_LANE.md)
- [CURSOR_FACTORY_LIAISON.md](CURSOR_FACTORY_LIAISON.md)
- [DiskLordz-Audio/README.md](../DiskLordz-Audio/README.md) — pointer to the private product repo
