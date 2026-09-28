# Hermes platform toolkit

CLI helpers for **Tier 2 + Tier 3** Hermes seats (ops, devops, handoff, gtm, presets, support, security, data).

## Build & run

```bash
python3 disklordz/hermes/scripts/hermes_tool.py --help
python3 disklordz/hermes/scripts/hermes_tool.py ops checklist
python3 disklordz/hermes/scripts/hermes_tool.py devops summary
python3 disklordz/hermes/scripts/hermes_tool.py handoff draft --wo WO-2026-HISE-001 --title "HISE bootstrap"
python3 disklordz/hermes/scripts/hermes_tool.py gtm brief --product junova --write
python3 disklordz/hermes/scripts/hermes_tool.py presets audit --product junova
python3 disklordz/hermes/scripts/hermes_tool.py support rag-status
python3 disklordz/hermes/scripts/hermes_tool.py security scan
python3 disklordz/hermes/scripts/hermes_tool.py data checklist
```

## Data flow

Each subcommand prints or writes **seat artifacts** under `disklordz/hermes/outbox/` — commit when WO-worthy.

## Agent repos (self-improvement)

Per-seat workspaces: [agent-repos/](agent-repos/README.md) · [docs/HERMES_AGENT_REPOS.md](../../docs/HERMES_AGENT_REPOS.md)

```bash
python3 disklordz/hermes/scripts/hermes_tool.py agent status
```

## Related

- [docs/HERMES_AGENT_FRAMEWORK.md](../../docs/HERMES_AGENT_FRAMEWORK.md)
- [.cursor/hermes/SKILLS_REGISTRY.md](../../.cursor/hermes/SKILLS_REGISTRY.md)
