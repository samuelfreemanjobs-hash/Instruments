# Disklordz DAW inbox watcher

**WO-SAAS-016** — optional desktop helper (no Electron required). Watches your **Downloads** folder for `disklordz-*.zip` files, extracts them into a **DAW inbox** folder, and archives the zip.

## Quick start

```bash
cd disklordz/daw-inbox
npm install
npm start
```

Default paths:

| | macOS / Linux default |
|--|--|
| Watch | `~/Downloads` |
| Inbox | `~/Music/Disklordz/Inbox/<zip-name>/` |
| Processed zips | `~/Music/Disklordz/Inbox/_processed_zips/` |

## Config file

Create `~/.config/disklordz/inbox.json`:

```json
{
  "downloadDir": "/path/to/Downloads",
  "inboxDir": "/path/to/DAW/Samples/Disklordz"
}
```

## Browser alternative (no install)

On the live site, after generating a kit, use **Save WAVs to folder** (Chrome/Edge) on the home page — writes files directly into a folder you pick.

## Run at login

- **macOS:** `launchd` plist pointing at `node …/bin/watch.mjs`
- **Linux:** user systemd unit
- **Windows:** Task Scheduler running `node watch.mjs`

See [ARCHITECTURE.md](ARCHITECTURE.md) for data flow.
