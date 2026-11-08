# Disklordz DAW inbox (WO-SAAS-016)

## Purpose

Move generated **Disklordz ZIPs** from the system Downloads folder into a stable **DAW inbox** directory so producers can drag WAVs into MPC, Ableton, Reaper, etc. without manual unzip.

## Build & run

```bash
cd disklordz/daw-inbox
npm ci
npm start          # watch Downloads
npm run once       # process existing zips once
```

Or: `npx --prefix disklordz/daw-inbox disklordz-inbox-watch`

## Data flow

```text
Browser → Download disklordz-*.zip → ~/Downloads
  → chokidar detects file
  → JSZip extract → ~/Music/Disklordz/Inbox/<zip-stem>/**
  → zip moved to Inbox/_processed_zips/
```

## Configuration

`~/.config/disklordz/inbox.json`:

```json
{
  "downloadDir": "/Users/you/Downloads",
  "inboxDir": "/Users/you/Music/Disklordz/Inbox",
  "processedDir": "/Users/you/Music/Disklordz/Inbox/_processed_zips"
}
```

Windows: `%USERPROFILE%\.config\disklordz\inbox.json` (full paths, escaped backslashes — see [README.md](README.md#windows-mpc-pc)).

Environment overrides: `DISKLORDZ_INBOX_DOWNLOAD_DIR`, `DISKLORDZ_INBOX_TARGET_DIR`, `DISKLORDZ_INBOX_PROCESSED_DIR`, `DISKLORDZ_INBOX_CONFIG`. Legacy misspelling `DISKLOORDZ_INBOX_*` is still accepted.

## Safety

`extractZip` resolves each entry against `<inboxDir>/<zip-stem>/` and skips entries that escape it (JSZip strips `../` but passes absolute names like `C:/…` through).

## Key modules

| Path | Role |
|------|------|
| `bin/watch.mjs` | Watcher + extract + archive zip |
| `package.json` | `disklordz-inbox-watch` CLI |

## Extension points

- Electron tray app wrapping the same script
- Authenticated pull from `/api/kits` instead of ZIP watch
- macOS Folder Actions / Windows Task Scheduler wrappers

## Related docs

- [README.md](README.md)
- [docs/DISKLORDZ_GO_LIVE.md](../../docs/DISKLORDZ_GO_LIVE.md)
- [disklordz/website/ARCHITECTURE.md](../website/ARCHITECTURE.md)
