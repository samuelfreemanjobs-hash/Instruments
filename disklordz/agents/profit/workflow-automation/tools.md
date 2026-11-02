# Tools — Workflow Automation

## Allowlist

Built-in: Read, Write, Edit, Grep, Glob, Bash

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

scaffold_workflows.py, scaffold-agent-workflows.yml

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
