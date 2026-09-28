# Tools — Audio Plugin Coder (APC)

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

https://github.com/Noizefield/audio-plugin-coder

## Automation entrypoints

docs/JUCE_APC_AGENT_BOOTSTRAP.md, root CMakeLists.txt, build-plugin.yml, APC submodule optional under disklordz/integrations/vendor/

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
