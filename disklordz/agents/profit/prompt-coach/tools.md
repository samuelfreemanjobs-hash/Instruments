# Tools — Prompt Coach

## Allowlist

Built-in: Read, Grep, WebFetch

MCP: supabase

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

https://github.com/vercel/ai

## Automation entrypoints

embed_and_upsert.py, hybrid RAG

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
