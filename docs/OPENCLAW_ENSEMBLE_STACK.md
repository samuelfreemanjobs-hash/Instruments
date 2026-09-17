# OpenClaw 2.0 ensemble stack — component map

DiskLordz vintage collective (1970s–1980s soul/jazz/R&B → phonk / screw / French touch / cyber funk) mapped to OpenClaw layers.

| Component / Layer | OpenClaw 2.0 implementation | Musical ensemble operational role |
|-------------------|----------------------------|-----------------------------------|
| **Gateway routing** | Declarative `openclaw.json` **`agents.entries`** — [`agents.config.disklordz.json`](../disklordz-factory/openclaw/agents.config.disklordz.json) merged at install | Enforces **workspace boundaries**, **model tiers** (Opus Maestro / Sonnet section), **`subagents.allowAgents`**, tool **allow/deny** (`sessions_spawn` only on Maestro) |
| **Persistence store** | Local **SQLite** (`openclaw-agent.sqlite` in Gateway state dir) + per-agent workspace files | Preserves **harmonic charts**, **lyrics/vocal maps**, **arrangement revisions**, fork **session logs** across turns; see [persistence.disklordz.md](../disklordz-factory/openclaw/persistence.disklordz.md) |
| **Persona layer** | Declarative **`SOUL.md`** + YAML personas [`openclaw/personas/`](../disklordz-factory/openclaw/personas/) | Era-specific **voicings**, **hardware**, **mix limits**, lyrical tropes — not generic aesthetic prompts ([persona engineering](OPENCLAW_PERSONAS.md)) |
| **Migration / stability** | OpenClaw **2.0** session SQLite + `openclaw doctor --session-sqlite inspect` | Prevents **lease deadlocks** on parallel spawn; **`disklordz_maestro`** `default: true` — [OPENCLAW_MIGRATION.md](OPENCLAW_MIGRATION.md) |
| **Tool execution** | **MCP skills** attached to Gateway or Cursor — [`factory_music_mcp`](../disklordz-factory/integrations/factory_music_mcp/) | **Scale/BPM lane checks**, **MIDI map formatting**, **`DL-BRF` brief schema** validation against artist rules |
| **Concurrency engine** | **`sessions_spawn`** with **`executionContext: "fork"`** — [`routing.config.disklordz.json`](../disklordz-factory/openclaw/routing.config.disklordz.json) | Forked background tasks so harmonic, drum step-seq, and vocal drafts **do not pollute** Maestro context; parallel Marcus ∥ Bernard ∥ Evelyn |

## Dual persistence (important)

| Store | Owns |
|-------|------|
| **OpenClaw** `openclaw-agent.sqlite` + workspaces | Agent sessions, Workboard, fork metadata, sectional drafts |
| **DiskLordz Factory** API + optional `disklordz_catalog.db` | `DL-OPP-*`, greenlights, catalog assets, approval queue |

Maestro **`exec`** calls Factory HTTP for research/catalog; sectional MCP tools read **repo seeds** (`artist_collective_seed.json`).

## Install checklist

```bash
./disklordz-factory/scripts/install-openclaw-workspace.sh
# merge agents.config.disklordz.json → ~/.openclaw/openclaw.json
# enable factory_music_mcp in Cursor — docs/FACTORY_MCP.md
```

## Related

- [OPENCLAW_GATEWAY_ROUTING.md](OPENCLAW_GATEWAY_ROUTING.md)
- [OPENCLAW_VINTAGE_COLLECTIVE.md](OPENCLAW_VINTAGE_COLLECTIVE.md)
- [FACTORY_MCP.md](FACTORY_MCP.md)
