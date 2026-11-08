# Tools — Plugin Factory Liaison

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

scripts/audio-plugin/factory-queue.py, PASTE_TO_FACTORY.md, .github/workflows/audio-plugin-fleet.yml

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
