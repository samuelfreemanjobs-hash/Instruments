# Tools — Analytics Interpreter

## Allowlist

Built-in: Read, Bash, Grep

MCP: stripe, supabase

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

https://github.com/stripe/agent-toolkit

## Automation entrypoints

Slack CI notify patterns, manual CSV export

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
