# Tools — Engine Swap

## Allowlist

Built-in: Read, Bash, Edit, Grep

MCP: none

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

https://github.com/facebookresearch/audiocraft

## Automation entrypoints

DISKLORDZ_ENGINE=remote, engine docker profile

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
