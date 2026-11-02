# Loop SOP — Churn Win-back

AsyncGenerator control plane for this agent (pseudocode).

```text
async function* query(state):
  while not done:
    state = compress(state)     # snip → microcompact → collapse → autocompact
    response = await stream(model, state)
    yield response.messages
    if not response.tool_calls:
      if stop_hook_allows: return completed
      continue
    batches = partition(response.tool_calls)  # per-invocation safety
    for batch in batches:
      results = await execute_batch(batch)    # 14-step pipeline
      yield results.messages
      state += results
```

## Terminals (discriminated)

`completed` | `max_turns` | `aborted_tools` | `prompt_too_long` | `permission_denied` | `hook_stopped`

## Compression budget

Trigger autocompact at `window - 13000` tokens; hard stop at `window - 3000`.

## Invariants

- Every `tool_use` paired with `tool_result` before next model call.
- Cancel: synthetic results for queued tools.
- Cost: track per-turn; sub-agents roll up to parent session.

## Primary verify command

```bash
bash disklordz/integrations/scripts/verify-integrations.sh
# or agent-specific checks in skill.md
```
