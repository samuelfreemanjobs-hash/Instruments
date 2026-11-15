# Tools — Linux VST3 DSP Engineer

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

tools/build-vst.sh, tools/build-vst-aarch64-linux.sh, scripts/vst/verify-mpc-sample-vst3.sh, python3 vst-testing-ops/run_business.py, docs/agents/LINUX_VST3_DSP_ENGINEER.md

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
