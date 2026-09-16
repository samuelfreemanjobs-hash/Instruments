# Agents — Disklordz / Instruments

Cloud Agents and humans working on **JD Upgraded** and repo tooling should follow this file plus the architecture index.

## Read first

1. [ARCHITECTURE.md](ARCHITECTURE.md) — repo layout and CI
2. [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) — plugin DSP and parameters
3. [docs/HANDOFF.md](docs/HANDOFF.md) — current phase notes
4. [docs/DISKLORDZ_INTEGRATIONS.md](docs/DISKLORDZ_INTEGRATIONS.md) — Slack, Linear, Telegram, Zapier, OpenClaw

## Build and verify

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j
./tests/golden/verify_golden.sh
```

CI runs the same path via [.github/workflows/build.yml](.github/workflows/build.yml).

## Cloud Agent environment

- Repo-managed config: [.cursor/environment.json](.cursor/environment.json)
- Validate: `./scripts/setup-disklordz-integrations.sh cursor-cloud`
- **Implementation** (code, PRs, CI fixes) belongs here — not in ChatGPT/Gemini chat alone.

## Linear MCP

Project config: [.cursor/mcp.json](.cursor/mcp.json) (Linear via `mcp-remote` → `https://mcp.linear.app/mcp`).

After pull, each developer must **enable Linear in Cursor → MCP** and complete **OAuth once** per machine.

## Triggering work from Telegram / GitHub

- **Queue a task (no code execution in Actions):** run workflow [Cloud agent task](.github/workflows/agent-task.yml) or send `repository_dispatch` event `agent-task` with JSON body `{ "task": "...", "source": "telegram" }`.
- That opens a GitHub issue labeled `cloud-agent` for you or a Cloud Agent run to pick up.
- **Personal Telegram** is alerts + short commands only — see OpenClaw section in [docs/DISKLORDZ_INTEGRATIONS.md](docs/DISKLORDZ_INTEGRATIONS.md).

## Scope discipline

- Minimize diff scope; match existing C++/JUCE style in `Source/` and `tools/`.
- No heap allocation on the audio thread.
- Update the relevant `ARCHITECTURE.md` when adding targets or major subsystems.
