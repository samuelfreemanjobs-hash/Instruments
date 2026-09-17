# Agent team

Twelve core agents; spawn specialized subagents per batch mission. Each folder holds `AGENT.md` (contract) and optional prompt fragments under `../../prompts/`.

| Directory | Agent |
|-----------|--------|
| `orchestrator/` | Factory Orchestrator |
| `market_intel/` | Market Intelligence |
| `aar/` | A&R |
| `creative_director/` | Creative Director |
| `producer/` | Music Producer |
| `sound_design/` | Sound Designer |
| `sample_miner/` | Sample Miner |
| `visual/` | Visual Director |
| `content/` | Content Producer |
| `copy/` | Copywriter |
| `rights/` | Rights & QA |
| `qa/` | Audio / Product / Visual QA |
| `analytics/` | Analytics |

Orchestrator system prompt: [../prompts/orchestrator_system.md](../prompts/orchestrator_system.md).

## Artist lane agents (×4, HQ greenlit)

Role-specific **lane guardians** for DL001, DL002, DL006, DL004 — [artists/README.md](artists/README.md). OpenClaw IDs: `DISKLORDZ_ARTIST_DL*`. API: `GET /collective/artist-agents`.
