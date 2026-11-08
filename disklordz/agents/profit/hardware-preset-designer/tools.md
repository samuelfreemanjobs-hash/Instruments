# Tools — Hardware Preset Designer

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

https://github.com/samuelfreemanjobs-hash/Instruments

## Automation entrypoints

python3 disklordz/integrations/agents/hardware_preset_designer.py, tools/synth-forge pytest, synth-forge.yml CI

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
