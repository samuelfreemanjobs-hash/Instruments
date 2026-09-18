# Claude Code — Instruments monorepo

You are a coding agent in the same factory as **Cursor Cloud**. Git is the source of truth; do not fight parallel agents on the same paths.

## Read first

1. [ARCHITECTURE.md](ARCHITECTURE.md) — product index  
2. [docs/AGENTIC_OPERATING_MODEL.md](docs/AGENTIC_OPERATING_MODEL.md) — lanes, gates, who runs where  
3. [docs/CLAUDE_CODE_INTEGRATION.md](docs/CLAUDE_CODE_INTEGRATION.md) — branch rules, handoffs, registry  
4. [docs/agent-registry.json](docs/agent-registry.json) — **which agent owns which product**  
5. Product doc for your task (e.g. [Wave909/ARCHITECTURE.md](Wave909/ARCHITECTURE.md), [disklordz/website/ARCHITECTURE.md](disklordz/website/ARCHITECTURE.md))

## New JUCE plugin work

Follow [docs/PLUGIN_FACTORY_OS.md](docs/PLUGIN_FACTORY_OS.md): lock `plugin.manifest.json` **before** `Source/`. Never rename `pluginName` from prompt examples.

## Build commands

**JD Upgraded + Wave909 (root CMake):**

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j
```

**MyFirstPlugin (standalone CMake):**

```bash
cd MyFirstPlugin && ./build.sh
```

**Disklordz SaaS:**

```bash
cd disklordz/website && npm ci && npm run build && npm test
```

## Git

- Feature branches: `claude/<description>-<suffix>` (same discipline as `cursor/*`)  
- One logical commit per change; **no force-push**; **no prod deploy** unless the user explicitly asks  
- Before large edits: `git fetch origin main` and confirm your product row in `docs/agent-registry.json`  
- Open or update a **draft PR**; do not merge to `main`

## Security

Same as [.cursor/rules/security-baseline.mdc](.cursor/rules/security-baseline.mdc): no secrets in git, validate API inputs, confirm before MCP/Zapier **writes**.

## Evidence

End tasks with **VERIFIED** (command you ran) or **NOT VERIFIED**. Default plugin release status: **RELEASE BLOCKED** with blockers listed.

## HISE / Antigravity

Do not run HISE export here if you are on Linux/Cloud. Use [disklordz/antigravity/](disklordz/antigravity/) handoff → Windows Antigravity per [docs/HISE_ANTIGRAVITY_LANE.md](docs/HISE_ANTIGRAVITY_LANE.md).
