# WO-SAAS-018 — Daily Factory DSP Cloud Agent (PM + ops)

**Owner:** Product / SaaS  
**Implementer:** Cursor Cloud Agent persona **Disklordz Factory DSP Engineer**  
**Schedule:** Daily **12:30 UTC** (GitHub Actions) → optional Cursor API launch  
**Repo:** `samuelfreemanjobs-hash/Instruments`  
**Branch pattern:** `cursor/factory-dsp-<theme>-4170`  
**PR title:** `WO-SAAS-018: Factory daily — <theme>`

## Purpose

Automate **continuous improvement** of the Drum Factory so customers get better kicks, snares, hats, loops, and packs **every day** without manual PM triage for each micro-improvement.

## Persona (three roles in one agent)

| Role | Focus |
|------|--------|
| **DSP engineer** | Filters, envelopes, saturation, stereo, quality gate |
| **Coding specialist** | TypeScript factory pipeline, tests, CI regression |
| **Sound designer** | Lane-accurate phonk/screw/MPC kits, musical loops |

Agent charter: [`.cursor/agents/disklordz-factory-dsp-engineer.md`](../.cursor/agents/disklordz-factory-dsp-engineer.md)

## Daily automation architecture

```text
GitHub cron (disklordz-factory-daily.yml)
  → factory:dsp-regression (offline, no secrets)
  → npm run build (website)
  → if CURSOR_API_KEY set:
       POST Cursor Cloud Agents API v1
       prompt = FACTORY_DAILY_THEME + charter + backlog link
       repos = Instruments @ main
       autoCreatePR = draft
  → optional Slack summary (SLACK_WEBHOOK_URL)
```

### One-time setup (human)

1. **Cursor:** Settings → Cloud Agents → connect GitHub → grant **Instruments** repo.
2. **API key:** Cursor Dashboard → API Keys → create key for automation.
3. **GitHub secret:** `CURSOR_API_KEY` on repo (Settings → Secrets → Actions).
4. **Optional:** `SLACK_WEBHOOK_URL` (same as CI) for daily run summaries.
5. **Optional:** Cursor Dashboard → **Automations** — duplicate schedule with the prompt in `disklordz/automation/prompts/factory-daily-improvement.md` if you want a second trigger path without GitHub.

Without `CURSOR_API_KEY`, the workflow still runs **regression + build** and fails if quality regresses.

## Theme rotation (UTC weekday)

| Day | `FACTORY_DAILY_THEME` | Typical work |
|-----|------------------------|--------------|
| Mon | `kick_808_sub_click` | Kick glide, sub, transient |
| Tue | `snare_clap_snap` | Snare body/snap, clap bursts |
| Wed | `hats_metallic_motion` | Open/closed, rolls, swing |
| Thu | `master_bus_lanes` | Grit, punch, stereo, quality gate |
| Fri | `loops_patterns` | Trap/screw patterns, ghost notes |
| Sat | `product_pack_consistency` | Folder packs, variation spread |
| Sun | `prompt_params_rag` | Prompt tokens → params; exemplars |

Backlog detail: [`disklordz/website/docs/FACTORY_IMPROVEMENT_BACKLOG.md`](../disklordz/website/docs/FACTORY_IMPROVEMENT_BACKLOG.md)

## Acceptance criteria (each daily PR)

- [ ] One clear customer-facing audio or reliability improvement
- [ ] `npm run factory:dsp-regression` passes in CI
- [ ] `npm run build` + `npm run lint` pass
- [ ] Draft PR with **Listen for** notes
- [ ] Line added to `FACTORY_IMPROVEMENT_LOG.md`

## One command (local — complete script)

From repo root:

```bash
chmod +x scripts/disklordz-factory-daily.sh
./scripts/disklordz-factory-daily.sh
```

From `disklordz/website`:

```bash
npm run factory:daily              # regression + build + lint + agent (if CURSOR_API_KEY)
npm run factory:daily:check        # verify tools, files, secrets
npm run factory:daily -- --regression-only
npm run factory:daily -- --dry-run --theme snare_clap_snap
```

## Manual trigger (GitHub)

```bash
gh workflow run disklordz-factory-daily.yml
gh workflow run disklordz-factory-daily.yml -f theme=loops_patterns
```

**Airtable / Zapier** → GitHub `repository_dispatch`:

```json
{
  "event_type": "disklordz-factory-daily",
  "client_payload": { "theme": "loops_patterns", "ref": "main" }
}
```

Until Factory v2 is on `main`, set `"ref": "cursor/factory-daily-agent-4170"` or GitHub secret env `DISKLORDZ_FACTORY_AGENT_REF` in the launch job.

## PM / Airtable

Create recurring WO **WO-SAAS-018** with checklist above. Link GitHub Action run URL and open draft PR. Close WO when PR is merged or explicitly deferred.

## Related docs

- [DISKLORDZ_FACTORY_V2.md](DISKLORDZ_FACTORY_V2.md) — engine map  
- [DISKLORDZ_ILLUGEN_RESEARCH.md](DISKLORDZ_ILLUGEN_RESEARCH.md) — WO table (018 added)  
- [disklordz/automation/README.md](../disklordz/automation/README.md) — dispatch patterns  
