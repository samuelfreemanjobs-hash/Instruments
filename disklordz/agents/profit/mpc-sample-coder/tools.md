# Tools — MPC Sample Coder

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

https://github.com/ioma8/mpc-sample-sdk

## Automation entrypoints

mpc_sample_coder.py --self-test, mpc-sample-ecosystem.yml, docs/MPC_SAMPLE_ECOSYSTEM.md

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
