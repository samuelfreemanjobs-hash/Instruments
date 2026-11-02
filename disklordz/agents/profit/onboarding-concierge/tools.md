# Tools — Onboarding Concierge

## Allowlist

Built-in: Read, Edit, Grep

MCP: none

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

https://github.com/inngest/inngest

## Automation entrypoints

/api/rag/suggest + magic link auth

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
