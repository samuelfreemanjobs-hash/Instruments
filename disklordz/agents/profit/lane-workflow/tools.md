# Tools — Lane Workflow

## Allowlist

Built-in: Read, Grep, Edit

MCP: none

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

https://github.com/langchain-ai/langgraph

## Automation entrypoints

langgraph_spec_flow.py, buildVariationBatch

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
