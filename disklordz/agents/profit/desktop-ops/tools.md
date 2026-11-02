# Tools — Desktop Ops

## Allowlist

Built-in: (MCP / external only)

MCP: none

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

https://github.com/bytebot-ai/bytebot

## Automation entrypoints

docs/BYTEBOT_SETUP.md

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
