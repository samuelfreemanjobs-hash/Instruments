# Tools — Conversion QA

## Allowlist

Built-in: Read, Grep, Glob, Bash, WebFetch

MCP: playwright

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

https://github.com/microsoft/playwright-mcp

## Automation entrypoints

verify-go-live.sh, scheduled CI

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
