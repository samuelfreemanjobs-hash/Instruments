# SP-1200 (VST) — architecture

**Primary implementer:** Claude Code (`claude/*` branches).  
**Factory:** [docs/PLUGIN_FACTORY_OS.md](../docs/PLUGIN_FACTORY_OS.md) · Manifest: [plugin.manifest.json](plugin.manifest.json)

## Purpose

VST drum instrument inspired by the E-mu SP-1200 workflow: sampling, 12-bit character, pads, and classic hip-hop/trap production use.

## Status

Scaffold on `main` for monorepo indexing. Active development may live on a **`claude/sp1200-*`** branch until first PR.

## Build (when CMake exists)

Will follow root `add_subdirectory(SP1200)` pattern like [Wave909/](../Wave909/).

## Coordination

- Do **not** collide with Cursor WIP on the same branch without handoff — see [docs/agent-registry.json](../docs/agent-registry.json).

## Related docs

- [docs/CLAUDE_CODE_INTEGRATION.md](../docs/CLAUDE_CODE_INTEGRATION.md)
