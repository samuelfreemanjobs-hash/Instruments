# Coach reaches you (not the other way around)

## 1. Phone app (Cursor publish — recommended)

Publish bundle: `disklordz/audio-command/mobile/`

1. In this chat, confirm the **Publish** dialog when the agent proposes **disklordz-coach**.
2. Choose **Private** (only you, signed in to Cursor).
3. On your phone, open the app from the Cursor chat pill or Apps list.
4. **Add to Home Screen** (iOS Share menu or Android Install).

## 1b. Local PWA (dev)

```bash
python3 -m http.server 8765 --bind 127.0.0.1 -d disklordz/audio-command/mobile
```

Open the URL → **Add to Home Screen**. Tap **Enable notifications** and pick a daily time.

The app shows **one task** with steps. **I did this — next task** advances the sprint. Cursor tasks include **Copy Cursor prompt**.

Export **Save progress for agent** → drop `progress.json` into `disklordz/audio-command/data/` (or paste into chat with audio-business-copilot).

## 2. Slack #disklordz (recommended)

Full map: [docs/SLACK_DISKLOORDZ.md](../../../docs/SLACK_DISKLOORDZ.md)

```bash
./scripts/setup-disklordz-integrations.sh slack-coach --webhook-url 'https://hooks.slack.com/services/...'
gh workflow run audio-coach-daily-nudge.yml -R samuelfreemanjobs-hash/Instruments
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
