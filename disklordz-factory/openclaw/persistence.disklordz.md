# Persistence — OpenClaw SQLite vs DiskLordz Factory

## OpenClaw Gateway (`openclaw-agent.sqlite`)

OpenClaw 2.0 persists **runtime agent state** in local SQLite (filename may vary by build; commonly **`openclaw-agent.sqlite`** under `OPENCLAW_STATE_DIR`, default `~/.openclaw`).

**Ensemble operational role:** preserve work across turns without re-loading full sectional context into the Maestro.

| Persisted by OpenClaw | Musical / ops content |
|------------------------|------------------------|
| Session & fork logs | Spawn lineage Maestro → Marcus/Bernard/Evelyn |
| Workboard plugin store | Batch cards, dependencies, proof attachments |
| Agent heartbeat / automation metadata | Night shift dispatch cadence |
| Optional transcript indexes | Pointers to workspace files |

**Not** the system of record for commercial catalog or YouTube greenlights — use Factory API.

## Workspace files (section isolation)

Long-form artifacts written under **`~/.openclaw/workspaces/`**:

| Path pattern | Content |
|--------------|---------|
| `vintage_keys/harmonic_charts/*.json` | Progressions, voicing rules, era tags |
| `rhythm_section/step_seq/*.json` | Bar grids, 808/909 roles, swing |
| `vocal_chops/stem_maps/*.json` | Phrase chops, provenance tags |
| `disklordz_maestro/briefs/DL-BRF-*.json` | Merged Maestro summaries (≤400-token source + links) |

Maestro AGENTS: reference **`artifact_paths`** in spawn deliverables, not inline prose.

## DiskLordz Factory (separate)

| Store | Path / API |
|-------|------------|
| In-memory (dev API) | Research, batches, approval queue |
| SQLite catalog (optional) | `disklordz-factory/database/disklordz_catalog.db` + `schema.sql` |
| Seeds (git) | `youtube_research_seed.json`, `artist_collective_seed.json` |

**Sync rule:** When a fork produces a shipping `DL-BRF-*` or `DL-KIT-*`, Maestro or operator registers assets via Factory `POST /catalog/assets` after Rights QA.

## Backup

- OpenClaw: backup `~/.openclaw/` (sqlite + workspaces)
- Factory: backup catalog DB + NAS tree per `docs/FOLDER_STRUCTURE.md`
