# Tools — Integration Health

## Allowlist

Built-in: Read, Bash, Grep

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

verify-integrations.sh, activate-integrations.yml

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
