# DRUMSMITH

Drum kit **product designer** and **DSP engineer** agent (four-layer FORGE-style package).

## Quick start

```bash
cd claude/drumsmith
python3 scripts/self_test.py
```

Claude Code: merge `.claude/` into repo root **or** run sessions with `cd claude/drumsmith` so `CLAUDE.md` loads.

```bash
# optional: symlink agents/skills into repo .claude
ln -sf ../../claude/drumsmith/.claude/agents/drumsmith.md .claude/agents/drumsmith.md
ln -sf ../../claude/drumsmith/.claude/agents/caliper.md .claude/agents/caliper.md
```

## First live kit

**Tail Light Static** — see `INTAKE.md` and `kits/tail-light-static/kit-spec.md`.

## ADR

- Stdlib-only verify scripts keep CI cheap and cross-platform.
- CALIPER stays read-only so QA cannot “fix” failing WAVs silently.
- Device profiles default UNVERIFIED to prevent false MPC/Maschine ship claims.