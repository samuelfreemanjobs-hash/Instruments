# Antigravity — active mode

Antigravity is a **parallel worker**, not a backup IDE. Active mode keeps a **queue of work** flowing without waiting for you to ask Cursor each time.

## How it works

```text
active-queue.json  →  dispatch-active.sh  →  inbox/HO-*.json  →  git push main
        ↑                                      │
        │                                      ▼
   completed_wo_ids                    Slack #disklordz-dev
                                               │
                                               ▼
                         Windows: watch-handoffs.ps1 (every 15 min)
                                               │
                                               ▼
                                    outbox/ + PR → Cursor reviews
```

## Cursor / Cloud (push work)

```bash
./scripts/antigravity-bridge/dispatch-active.sh --push          # next queue item
./scripts/antigravity-bridge/dispatch-active.sh --wo WO-AG-001 --push
./scripts/antigravity-bridge/dispatch-active.sh --dry-run
```

GitHub: workflow **`antigravity-dispatch-active.yml`** (schedule + manual).

## Windows (pull work — keep Antigravity hot)

```powershell
.\scripts\antigravity-bridge\install-antigravity-workspace.ps1
.\scripts\antigravity-bridge\watch-handoffs.ps1 -Once   # now
.\scripts\antigravity-bridge\watch-handoffs.ps1          # loop every 15 min
```

**Task Scheduler (recommended):** run `watch-handoffs.ps1 -Once` every 15 minutes at logon.

## Lanes (not only HISE)

| Lane | Agent id | Examples |
|------|-----------|----------|
| `batch` | antigravity-batch | Presets, manifests, Python batch audio |
| `docs` | antigravity-docs | Large specs, genre JSON packs |
| `hise` | antigravity-hise | HISE sketch exports |

Edit queue: `disklordz/antigravity/active-queue.json`.

## Slack

When `HO-*.json` lands on **`main`**, workflow **`antigravity-inbox-slack.yml`** pings **#disklordz-dev** (if webhook configured).

## Cursor rule

`.cursorrules` — for batch/docs/heavy tasks, **dispatch Antigravity** (packet or `dispatch-active.sh`) instead of doing the work inline.

## Product creation (prep factory)

Sprint-aware auto-dispatch: [ANTIGRAVITY_PRODUCT_FACTORY.md](ANTIGRAVITY_PRODUCT_FACTORY.md) · `sync-product-prep.py`

## Related

- [disklordz/antigravity/ARCHITECTURE.md](../disklordz/antigravity/ARCHITECTURE.md)
- [disklordz/antigravity/AGENT_INSTRUCTIONS.md](../disklordz/antigravity/AGENT_INSTRUCTIONS.md)
