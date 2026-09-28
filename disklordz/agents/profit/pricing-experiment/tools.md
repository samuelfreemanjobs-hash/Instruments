# Tools — Pricing Experiment

## Allowlist

Built-in: Read, Grep

MCP: stripe

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

https://github.com/stripe/agent-toolkit

## Automation entrypoints

STRIPE_PRO_PRICE_ID env variants

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
