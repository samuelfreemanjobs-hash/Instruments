# Tools — PM Agent (Fleet ADD)

## Allowlist

Built-in: Read, Write, Edit, Grep, Glob, Bash

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

agent-fleet-governance.yml, agent-fleet-execute.yml, sync-disklordz-agent-fleet.sh

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
