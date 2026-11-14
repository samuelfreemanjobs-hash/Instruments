# Drum machine engines (synthesized factory)

Canonical engine ids (procedural only — no sample packs):

| Engine id | Character |
|-----------|-----------|
| `hard_dirt_808` | Dirty Memphis 808, cassette grit |
| `traproom_tape` | SP-1200-style truncation, trap weight |
| `tape_slowdown` | Screw/slow tape pitch drag |
| `tr808` / `tr909` | Classic drum machine voicing |
| `boss_dr660` / `alesis_sr16` / `roland_r8mk2` | 90s workstation grids |
| `mr_tape` | Collage / splice tape |

Engine alias `tape_screw` → `tape_slowdown` in `phonk_machine_engine.ENGINE_LEGACY_ALIASES`.

Prompt tags `traproom`, `hard dirt`, `sp 1200`, `memphis phonk` select engines in `resolve_engine_id()`.
