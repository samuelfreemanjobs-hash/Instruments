# Tools — Airtable WO Triage

## Allowlist

Built-in: Read, Grep, Bash

MCP: github

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

https://github.com/github/github-mcp-server

## Automation entrypoints

airtable-antigravity-handoff.yml, disklordz/antigravity/inbox/

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
