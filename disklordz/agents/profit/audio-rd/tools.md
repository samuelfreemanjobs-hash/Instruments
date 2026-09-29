# Tools — Audio R&D

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

https://github.com/google/ddsp

## Automation entrypoints

docs/RD_EXPERIMENT_LOG.md, tools/drum-synth-blueprint/, docs/DDSP_TRAP_PHONK_AGENT_PROMPTS.md, disklordz/rag corpus

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
