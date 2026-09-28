# Hooks — Lane Workflow

Lifecycle interceptors (27-event model; implement v0 with these).

| Event | Type | Action |
|-------|------|--------|
| SessionStart | command | `python3 disklordz/rag/scripts/chunk_corpus.py` (if docs changed) |
| UserPromptSubmit | prompt | Reject prompts asking to exfiltrate secrets or disable RLS |
| PreToolUse | command | Block `Bash(rm -rf`, `Bash(curl * | bash`, force-push git |
| PreToolUse | prompt | For Stripe/Supabase **writes**, require explicit user confirmation |
| PostToolUse | command | Append last command to `~/.claude/projects/disklordz/memory/lane-workflow_session.log` |
| Stop | prompt | If task incomplete, list remaining steps before allowing stop |

## Snapshot

Freeze hook config at session start; update only via user `/hooks` or repo change + restart.

## Exit codes (command hooks)

- `0` success
- `2` block
- other = warning only
