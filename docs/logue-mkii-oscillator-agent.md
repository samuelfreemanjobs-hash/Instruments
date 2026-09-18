# logue SDK mkII custom oscillator agent

**Purpose:** Design and implement **custom user oscillators** for **NTS-1 mkII** (logue SDK **v2**). Optional exports: v1.1 units for Minilogue XD / NTS-1 mkI / Prologue.

**Not in scope:** NTS hardware design, firmware, or non-oscillator logue modules (unless explicitly requested).

## Naming

| Term | Meaning |
|------|---------|
| **logue SDK custom oscillator agent** | Umbrella: user-osc slot, Korg toolchain |
| **mkII standard** | Default deliverable: `src/mkii/oscillators/<slug>/` → `.nts1mkiiunit` |
| **v1.1 legacy** | `src/oscillators/<name>/` → `.prg` / compatible ELF |

## Agent entry points

- System prompt: [.cursor/agents/logue-mkii-oscillator/SYSTEM.md](../.cursor/agents/logue-mkii-oscillator/SYSTEM.md)
- Skill: [.cursor/skills/logue-hardware-port/SKILL.md](../.cursor/skills/logue-hardware-port/SKILL.md)
- Golden unit: [logue-mkii-golden-unit.md](logue-mkii-golden-unit.md)

## Workflow

```text
Sound brief + parameter plan (≤10)
  → scaffold (tools/mkii/scaffold-mkii.py)
  → implement osc.h (+ setParameter 0–1023)
  → ./tools/osc-eval-mkii.sh <slug>
  → ./tools/build-mkii.sh <slug>  (if eval passed static + LOGUE_SDK present)
  → load on mkII (Kontrol Editor / loguecli / SDK docs)
```

## Evaluation

```bash
./tools/osc-eval-mkii.sh tr808_kick_phonk           # golden
./tools/osc-eval-mkii.sh my_new_slug --static-only  # no SDK; API grep only
```

Exit codes: `0` pass, `1` static failure, `2` build skipped (no SDK), `3` build failed.

## Related docs

- [nts1-mkii-porting.md](nts1-mkii-porting.md)
- [nts1-multi-bass-oscillators.md](nts1-multi-bass-oscillators.md)
- [platform-matrix.md](platform-matrix.md)
- [AGENTS.md](../AGENTS.md)

## Quarantined Claude bundle

Outdated third-party prompts (`osc_process`, `user_osc.c`, wrong MCU tables) live under [archive/quarantined-claude-osc-bundle/](archive/quarantined-claude-osc-bundle/) — **do not use for codegen**.
