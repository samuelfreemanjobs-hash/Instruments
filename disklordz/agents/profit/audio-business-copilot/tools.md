# Tools — Audio Business Copilot

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

https://github.com/samuelfreemanjobs-hash/Instruments

## Automation entrypoints

.github/workflows/audio-plugin-fleet.yml, audio-coach-daily-nudge.yml, audio-plugin-weekly.yml, scripts/run-audio-plugin-fleet.sh

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
