# Tools — Support Macro

## Allowlist

Built-in: Read, Grep

MCP: none

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

https://github.com/anthropics/skills

## Automation entrypoints

.github/skills/disklordz-rag

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
