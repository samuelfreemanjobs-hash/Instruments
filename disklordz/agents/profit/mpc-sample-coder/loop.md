# Loop SOP — MPC Sample Coder

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
python3 disklordz/integrations/agents/mpc_sample_coder.py --self-test
python3 disklordz/trap-lord-synth/tests/test_mpc_xpm_export.py
```
