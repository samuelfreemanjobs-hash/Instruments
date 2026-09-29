# Tools — Code Project Planner (PRD)

## Allowlist

Built-in: Read, Write, Edit, Grep, Glob

MCP: none

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

https://github.com/samuelfreemanjobs-hash/Instruments

## Automation entrypoints

docs/templates/PRD_TEMPLATE.md, docs/COMPANY_MEMORY_INDEX.md, per-product docs/PRD.md or product PRD section

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
