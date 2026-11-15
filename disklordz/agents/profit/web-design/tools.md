# Tools — Web Design

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

https://stitch.withgoogle.com

## Automation entrypoints

disklordz/website/docs/WEB_DESIGN_OPS.md

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
