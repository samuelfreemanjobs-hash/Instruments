# Audio plugin agent lane

Operator map for **CHORD ENGINE / Milestone 001** — agents already in this repo, wired for you.

## Agent roster (audio)

| Agent | Role | Automation |
|-------|------|------------|
| **audio-business-copilot** | Sprint, master plan, weekly narrative | Coach PWA + `audio-coach-daily-nudge.yml` |
| **plugin-factory-liaison** | Next Factory slice, no C++ | Reads `PASTE_TO_FACTORY.md` |
| **autonomous-coding-agent** | 24/7 allowlisted repo work | `autonomous-coding-agent.yml` |
| **golden-wav-qa** | Plugin DSP regression | `build.yml` + optional `audio-plugin-fleet.yml` |
| **license-compliance** | JUCE / commercial gate | Manual before launch |
| **airtable-wo-triage** | WO → Antigravity / issues | `airtable-antigravity-handoff.yml` |

**Windows / prep:** Antigravity (`sync-all-antigravity.sh`) · **Terminal DPF:** Claude Code + `request-claude-support.sh`

## Daily (you do almost nothing)

1. **Coach** — one human task (e.g. dev-environment).
2. **GitHub 15:30 UTC** — `audio-plugin-fleet.yml` refreshes **`disklordz/audio-command/data/PASTE_TO_FACTORY.md`**.
3. **Slack #disklordz** — coach nudge (`audio-coach-daily-nudge.yml` or Cursor timer).
4. **Antigravity** — Windows `watch-handoffs.ps1` every 15 min.

## When Factory should code

```bash
cat disklordz/audio-command/data/PASTE_TO_FACTORY.md
# Paste into Cursor Factory OR invoke skill disklordz-plugin-factory-liaison
```

Blocked until **`dev-environment`** is done in Coach — file will say `human-windows`.

## Local one-shot

```bash
python3 scripts/audio-plugin/fleet_tick.py --weekly --antigravity
```

## Prep handoff bundle

[AUDIO_PREP_HANDOFF_BUNDLE.md](AUDIO_PREP_HANDOFF_BUNDLE.md) — paths after WO-PREP-001/002.

## Operator (Pro plan)

[CURSOR_OPERATOR_ON_20_PRO.md](CURSOR_OPERATOR_ON_20_PRO.md) — compute tiers, billing guardrails, single inbox.

## Related

- [CURSOR_FACTORY_LIAISON.md](CURSOR_FACTORY_LIAISON.md)
- [AUTONOMOUS_CODING_AGENT.md](AUTONOMOUS_CODING_AGENT.md)
- [DISKLORDZ_MASTER_PLAN.md](DISKLORDZ_MASTER_PLAN.md)
