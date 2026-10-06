# Tools — Churn Win-back

## Allowlist

Built-in: Read, Grep, WebFetch

MCP: supabase, stripe

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

https://github.com/n8n-io/n8n

## Automation entrypoints

saved_kits + billing snapshot queries

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
