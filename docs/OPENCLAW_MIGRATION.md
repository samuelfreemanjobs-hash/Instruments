# OpenClaw 2.0 — migration & multi-agent stability

DiskLordz runs a **parallel subagent ensemble** (Maestro + Marcus/Bernard/Evelyn forks). OpenClaw **2.0** normalizes session state into SQLite; unmigrated configs can cause **lease-acquisition deadlocks** on parallel `sessions_spawn`.

## Pre-flight checklist

1. **Upgrade Gateway** to 2.0 runtime series (session SQLite normalized).
2. **Migrate state** — resolve legacy session paths; do not run old multi-agent topology alongside unmigrated stores.
3. **Install DiskLordz workspace:**
   ```bash
   ./disklordz-factory/scripts/install-openclaw-workspace.sh
   ```
4. **Merge** `agents.config.disklordz.json` → `~/.openclaw/openclaw.json`.
5. **Primary coordinator:** ensure **`disklordz_maestro`** has `"default": true` (already set in repo template).
6. **Inspect DB health:**
   ```bash
   openclaw doctor --session-sqlite inspect
   openclaw doctor --fix   # materialize stale monitor rows if prompted
   ```
7. **Restart Gateway** before parallel composition cycles.

## Deadlock avoidance

| Risk | Mitigation |
|------|------------|
| Legacy config paths | Single `OPENCLAW_STATE_DIR`; reinstall workspace; no duplicate agent ids |
| Parallel spawn without migration | Run `doctor --session-sqlite inspect` until clean |
| Multiple defaults | Only **`disklordz_maestro`** is `default: true` |
| Section agents spawning | **`deny: sessions_spawn`** on Marcus/Bernard/Evelyn (config enforced) |
| Lease contention | Cap parallel forks to 3 (OpenClaw default dispatch); Maestro merges before next wave |

## Persistence split

| Store | File | Role |
|-------|------|------|
| OpenClaw sessions | `openclaw-agent.sqlite` | Fork logs, leases, scheduling |
| Workboard | Plugin SQLite in Gateway | Card DAG |
| Section drafts | `~/.openclaw/workspaces/*/harmonic_charts` etc. | See [persistence.disklordz.md](../disklordz-factory/openclaw/persistence.disklordz.md) |
| Factory catalog | API / optional `disklordz_catalog.db` | `DL-OPP-*`, assets, approval |

## After migration

- Run a **dry fork**: Maestro spawns `vintage_keys` with `executionContext: fork`, verify completion without gateway timeout.
- Factory: `GET /research/status` still independent of OpenClaw SQLite.

## Related

- [OPENCLAW_ENSEMBLE_STACK.md](OPENCLAW_ENSEMBLE_STACK.md)
- [OPENCLAW_GATEWAY_ROUTING.md](OPENCLAW_GATEWAY_ROUTING.md)
