# Tools — Competitive Intel

## Allowlist

Built-in: Read, WebFetch, Grep, Write

MCP: none

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

https://github.com/entropyvortex/meta-llm-charter

## Automation entrypoints

docs/DISKLORDZ_ILLUGEN_RESEARCH.md

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
