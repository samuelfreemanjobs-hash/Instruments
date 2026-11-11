# Claude Code → Antigravity requests

Claude Code (terminal) drops **support requests** here. Antigravity on Windows executes; results land in `outbox/` and `outbox-artifacts/claude-code/`.

## Create a request (any OS)

```bash
./scripts/antigravity-bridge/request-claude-support.sh \
  --slice "DPF-MIDI triad unit tests" \
  --kind pytest_run \
  --dpf DPF-MIDI \
  --path docs/CHORD_ENGINE_PRD.md
```

Optional `--push` to commit and notify Slack (on `main`).

## Request schema (`CC-*.json`)

| Field | Meaning |
|-------|---------|
| `support_kind` | `windows_build` · `pytest_run` · `codegen_stub` · `midi_fixture` · `cmake_scaffold` |
| `dpf_area` | `DPF-Core` · `DPF-MIDI` · `DPF-DSP` · `Build` · `Tests` |
| `slice` | One sentence — current Claude Code slice |
| `context_paths` | Docs to read |

## Pull results (Windows)

```powershell
.\scripts\antigravity-bridge\watch-handoffs.ps1 -Once
git pull
ls disklordz\antigravity\outbox
```

See [docs/ANTIGRAVITY_CLAUDE_CODE_COPILOT.md](../../docs/ANTIGRAVITY_CLAUDE_CODE_COPILOT.md).
