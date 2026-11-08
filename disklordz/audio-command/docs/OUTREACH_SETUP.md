# Coach reaches you (not the other way around)

## 1. Install the Coach PWA (phone or desktop)

```bash
python3 -m http.server 8765 --bind 127.0.0.1 -d disklordz/audio-command/pwa
```

Open the URL → **Add to Home Screen**. Tap **Enable notifications** and pick a daily time.

The app shows **one task** with steps. **I did this — next task** advances the sprint. Cursor tasks include **Copy Cursor prompt**.

Export **Save progress for agent** → drop `progress.json` into `disklordz/audio-command/data/` (or paste into chat with audio-business-copilot).

## 2. Slack / Discord daily DM (recommended)

1. Create an [Slack Incoming Webhook](https://api.slack.com/messaging/webhooks) (or Discord webhook).
2. Add GitHub repo secret: `DISKLORDZ_COACH_WEBHOOK_URL`
3. Set repo variable: `AUDIO_COACH_NUDGES_ENABLED` = `true`
4. Edit schedule in `.github/workflows/audio-coach-daily-nudge.yml` (cron) to your timezone.

Manual test:

```bash
DISKLORDZ_COACH_WEBHOOK_URL='https://hooks.slack.com/...' \
  python3 disklordz/audio-command/scripts/coach_outreach.py --send
```

## 3. iPhone / Google Calendar passive nudge

```bash
python3 disklordz/audio-command/scripts/coach_calendar.py --write
```

AirDrop or sync `disklordz/audio-command/exports/coach-daily.ics` → Subscribe on phone. Description contains today's steps.

## 4. Cursor agent pings you on a schedule

In a chat with **audio-business-copilot**, ask to subscribe a daily timer that runs:

`python3 disklordz/audio-command/scripts/coach_outreach.py` and presents the result to you.

(Uses Cursor **subscribe_timer** — see subscribe skill.)

## 5. What the agent must do every session

Per `audio-business-copilot` soul: **open with your single next task** (from `coach_engine.py`), not a wall of options. You confirm or mark done; agent updates `data/progress.json` when you export progress.
