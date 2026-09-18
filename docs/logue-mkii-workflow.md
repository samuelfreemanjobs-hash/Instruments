# logue SDK mkII custom oscillator workflow

**Agent:** logue SDK mkII custom oscillator agent · **Product:** NTS-1 mkII (`.nts1mkiiunit`)

## Slash commands

| Command | Phase |
|---------|--------|
| `/logue-mkii-setup` | Clone SDK, ARM gcc (`mkii-automate.sh bootstrap`) |
| `/logue-mkii-brief <slug>` | Sound brief + parameter plan (≤10) |
| `/logue-mkii-design <slug>` | Algorithm, anti-aliasing, param curves |
| `/logue-mkii-scaffold <slug>` | `scaffold-mkii.py` → unit tree |
| `/logue-mkii-impl <slug>` | Implement `osc.h` DSP |
| `/logue-mkii-eval <slug>` | `tools/osc-eval-mkii.sh` |
| `/logue-mkii-ship <slug>` | Build + load checklist |
| `/logue-mkii-debug <slug>` | Fix build/DSP issues |
| `/logue-mkii-new <slug>` | Full pipeline with confirmations |
| `/logue-mkii-status <slug>` | Read `.agent/status.json` |
| `/logue-mkii-resume <slug>` | Continue last incomplete phase |

Aliases: `/mkii-osc-brief` = `/logue-mkii-brief`, etc.

## One-shot prompt (no slash)

> Use the logue mkII oscillator agent to design a [sound] for NTS-1 mkII. Slug: `my_unit`.

Agent should still follow phases internally and run eval before calling done.

## State file

`src/mkii/oscillators/<slug>/.agent/status.json`:

```json
{
  "slug": "my_unit",
  "current_phase": "design",
  "setup_completed": true,
  "v1_source": "oscillators/dilla-bass",
  "phases_completed": ["brief", "design"]
}
```

## Files per unit

| Phase | Outputs |
|-------|---------|
| brief | `.agent/brief.md` |
| design | `.agent/design.md` |
| scaffold | `Makefile`, `osc.h`, `unit.cc`, … |
| impl | `osc.h` (PORT_COMPLETE), `PORTING.md` |
| eval | log from `osc-eval-mkii.sh` |
| ship | `.agent/ship.md` (load steps, artifact path) |

## Build commands

```bash
./tools/mkii-automate.sh bootstrap
./tools/osc-eval-mkii.sh tr808_kick_phonk --static-only
./tools/osc-eval-mkii.sh <slug>
./tools/build-mkii.sh <slug>
```

## Related

- [logue-mkii-oscillator-agent.md](logue-mkii-oscillator-agent.md)
- [.cursor/agents/logue-mkii-oscillator/SYSTEM.md](../.cursor/agents/logue-mkii-oscillator/SYSTEM.md)
