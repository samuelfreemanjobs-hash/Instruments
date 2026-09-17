# OpenClaw gateway routing — ensemble isolation & spawn protocol

DiskLordz vintage collective uses **gateway routing** to separate the bandleader’s compositional state from verbose sectional work (harmony, drums, vocal chops).

## Design goals

| Concern | Policy |
|---------|--------|
| **Model tiering** | Maestro = high-reasoning (Opus); section leaders + lane guardians = Sonnet |
| **Workspace isolation** | Each `agents.entries.*.workspace` is a distinct directory; sandbox `scope: agent` |
| **Context hygiene** | Long harmonic, drum-sequencing, and vocal drafts stay in **forked subagent sessions** |
| **Maestro memory** | Orchestrator receives **summaries + artifact paths only**, not full sectional threads |

## Configuration source

- Agent entries: [`disklordz-factory/openclaw/agents.config.disklordz.json`](../disklordz-factory/openclaw/agents.config.disklordz.json)
- Spawn protocol (machine-readable): [`disklordz-factory/openclaw/routing.config.disklordz.json`](../disklordz-factory/openclaw/routing.config.disklordz.json)
- Maestro procedures: [`disklordz-factory/openclaw/workspaces/disklordz_maestro/AGENTS.md`](../disklordz-factory/openclaw/workspaces/disklordz_maestro/AGENTS.md)

## Model tiering

```text
Tier 0 (orchestration)  disklordz_maestro     claude-opus-4-6   briefs, sync, batch structure
Tier 1 (section)        vintage_keys          claude-sonnet-4-6 harmonic depth
                        rhythm_section        claude-sonnet-4-6 drum / step-seq depth
                        vocal_chops           claude-sonnet-4-6 vocal draft depth
Tier 1 (gate)           market_intel, aar     claude-sonnet-4-6 research / decisions
Tier 1 (lane)           artist_dl*            claude-sonnet-4-6 lane validation only
```

## Tool permissions

| Agent | allow | deny (must not spawn or shell) |
|-------|-------|--------------------------------|
| `disklordz_maestro` | `read`, `write`, `sessions_spawn`, `exec` | — |
| Section + artist + A&R | `read`, `write` | `sessions_spawn`, `exec` |
| `market_intel` | `read`, `write`, `exec` | `sessions_spawn` |

Instrumentalists **cannot** spawn further subagents — prevents unbounded context forks from the rhythm or keys thread.

## `sessions_spawn` — fork vs inherit

Use **`sessions_spawn`** from the Maestro only.

| Context | When | Maestro keeps |
|---------|------|----------------|
| **`fork`** (default) | Harmonic analysis, 16-bar step-seq drafts, vocal phrase maps, lane validation | Task card + **≤400 token summary** + file paths |
| **`inherit`** | Rare: quick yes/no from `artist_dl00x` on a single rule | Short answer only |
| **Never inherit** | `vintage_keys`, `rhythm_section`, `vocal_chops` verbose passes | — |

### Spawn payload pattern (Maestro)

```json
{
  "agent": "vintage_keys",
  "executionContext": "fork",
  "task": "DL-BRF harmonic map for artist_dl002 / batch card {id}. BPM 148. Memphis soul chop behavior only.",
  "deliverable": {
    "format": "json",
    "max_tokens": 400,
    "fields": ["key", "progression_sketch", "voicing_rules", "forbidden", "artifact_paths"]
  },
  "do_not_return": ["full conversation", "exploratory prose", "alternative progressions beyond top 2"]
}
```

Parallel spawns (same batch, after lane guardian OK):

1. `vintage_keys` — fork  
2. `rhythm_section` — fork  
3. `vocal_chops` — fork  

Maestro **merges** the three JSON summaries into one `DL-BRF-*` attachment on the Workboard card.

## Inter-agent synchronization

1. Maestro holds **batch state**: mission, `artist_id`, Workboard parent id, Factory `batch_id`.
2. Section forks **must not** call Factory greenlight APIs — only `market_intel` / `aar` entries do.
3. Lane guardian (`artist_dl00x`) validates merged brief **before** Sample Miner / night shift.
4. Workboard `workboard_complete` on section children attaches proof files; Maestro links parent.

## Factory API (out of band)

Subagent forks do not replace Factory catalog state. Maestro or `exec` from Maestro session:

- `GET /research/status`
- `GET /collective/artist-agents/{id}`
- `POST /night-shift/run` (after QA path)

## Install

```bash
./disklordz-factory/scripts/install-openclaw-workspace.sh
# merge agents.config.disklordz.json → openclaw.json
```

Restart Gateway after routing changes.

## Related

- [OPENCLAW_VINTAGE_COLLECTIVE.md](OPENCLAW_VINTAGE_COLLECTIVE.md)
- [openclaw/MERGE.md](../disklordz-factory/openclaw/MERGE.md)
