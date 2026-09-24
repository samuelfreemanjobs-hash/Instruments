# VST factory outbox — completion records

When an agent finishes a handoff from `inbox/HO-*.json`, write **`HO-<same-id>-done.json`** here (or `HO-<id>-done.json` referencing `handoff_id`).

Required fields: `handoff_id`, `status` (`done` | `blocked`), `completed_at` (ISO UTC), `agent`, `result` (`pass` | `fail` | `partial`), optional `pr_url`, `notes`.

See [docs/AUTOMATED_WORK_COMPLETION.md](../../docs/AUTOMATED_WORK_COMPLETION.md).
