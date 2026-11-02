# Tools — Product Factory

## Allowlist

Built-in: Read, Write, Edit, Grep, Bash

MCP: none

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

https://github.com/crewAIInc/crewAI

## Automation entrypoints

crew_product_factory.py, POST /api/factory/batch

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
