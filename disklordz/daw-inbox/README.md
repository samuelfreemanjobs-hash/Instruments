# Disklordz DAW inbox watcher

**WO-SAAS-016** — optional desktop helper (no Electron required). Watches your **Downloads** folder for `disklordz-*.zip` files, extracts them into a **DAW inbox** folder, and archives the zip.

Requires **Node.js 18+** (`node --version`).

## Quick start

```bash
cd disklordz/daw-inbox
npm ci
npm run once   # process zips already in Downloads, then exit
npm start      # keep watching (Ctrl+C to stop)
```

Default paths:

| | Windows default | macOS / Linux default |
|--|--|--|
| Watch | `%USERPROFILE%\Downloads` | `~/Downloads` |
| Inbox | `%USERPROFILE%\Music\Disklordz\Inbox\<zip-name>\` | `~/Music/Disklordz/Inbox/<zip-name>/` |
| Processed zips | `%USERPROFILE%\Music\Disklordz\Inbox\_processed_zips\` | `~/Music/Disklordz/Inbox/_processed_zips/` |
| Config file | `%USERPROFILE%\.config\disklordz\inbox.json` | `~/.config/disklordz/inbox.json` |

The script prints the resolved paths on start — check them before pointing it at a real MPC sample folder.

## Windows (MPC PC)

```powershell
cd C:\path\to\Instruments\disklordz\daw-inbox
npm ci
npm run once
```

Expected output starts with `[disklordz-inbox] config` and the three paths above, then `--once complete`.

### Optional config (`inbox.json`)

Only needed if you want paths other than the defaults (e.g. straight into your MPC expansion folder).

```powershell
New-Item -ItemType Directory -Force "$env:USERPROFILE\.config\disklordz" | Out-Null
notepad "$env:USERPROFILE\.config\disklordz\inbox.json"
```

```json
{
  "downloadDir": "C:\\Users\\YOU\\Downloads",
  "inboxDir": "C:\\Users\\YOU\\Music\\Disklordz\\Inbox"
}
```

- `%USERPROFILE%` is **not** expanded inside JSON — write the full path.
- Backslashes must be doubled (`\\`), or use forward slashes (`C:/Users/YOU/Downloads`).
- Optional `processedDir` (default: `<inboxDir>\_processed_zips`).

### Run at login (Task Scheduler)

1. Find the full path to Node: `where.exe node` (typically `C:\Program Files\nodejs\node.exe`).
2. In an **Administrator** PowerShell (logon triggers usually need elevation), create the task — replace both paths with yours:

   ```powershell
   $action  = New-ScheduledTaskAction -Execute "C:\Program Files\nodejs\node.exe" -Argument '"C:\path\to\Instruments\disklordz\daw-inbox\bin\watch.mjs"'
   $trigger = New-ScheduledTaskTrigger -AtLogOn -User $env:USERNAME
   Register-ScheduledTask -TaskName "Disklordz DAW Inbox" -Action $action -Trigger $trigger
   ```

   Same thing from **cmd.exe** (not PowerShell — the `\"` escaping is cmd syntax):

   ```bat
   schtasks /Create /TN "Disklordz DAW Inbox" /SC ONLOGON /F /TR "\"C:\Program Files\nodejs\node.exe\" \"C:\path\to\Instruments\disklordz\daw-inbox\bin\watch.mjs\""
   ```

3. Test without logging out: `Start-ScheduledTask -TaskName "Disklordz DAW Inbox"` — a console window opens and shows `watching…`. Download a kit; it should land in the inbox folder.
4. Remove later: `Unregister-ScheduledTask -TaskName "Disklordz DAW Inbox" -Confirm:$false`.

The working directory does not matter (dependencies resolve next to `bin\watch.mjs`), but run `npm ci` in `disklordz\daw-inbox` first. Closing the console window stops the watcher until next login.

**No admin?** Press `Win+R` → `shell:startup` → create a shortcut with target `"C:\Program Files\nodejs\node.exe" "C:\path\to\Instruments\disklordz\daw-inbox\bin\watch.mjs"`.

## Environment overrides

`DISKLORDZ_INBOX_DOWNLOAD_DIR`, `DISKLORDZ_INBOX_TARGET_DIR`, `DISKLORDZ_INBOX_PROCESSED_DIR`, `DISKLORDZ_INBOX_CONFIG` (take precedence over `inbox.json`). The older misspelled `DISKLOORDZ_INBOX_*` names still work.

## Browser alternative (no install)

On the live site, after generating a kit, use **Save WAVs to folder** (Chrome/Edge) on the home page — writes files directly into a folder you pick.

## Run at login (other OS)

- **macOS:** `launchd` plist pointing at `node …/bin/watch.mjs`
- **Linux:** user systemd unit

## Safety

Zip entries that would extract outside `<inboxDir>\<zip-name>\` (`..\` or absolute paths such as `C:/…`) are skipped with a warning.

See [ARCHITECTURE.md](ARCHITECTURE.md) for data flow.
