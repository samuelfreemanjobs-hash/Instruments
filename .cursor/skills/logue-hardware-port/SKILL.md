---
name: logue-hardware-port
description: mkII-first logue SDK v2 custom oscillators (NTS-1 mkII). Secondary v1.1 port for XD/Prologue/mkI NTS. Use with logue mkII oscillator agent — not for JUCE-only plugins.
---

# logue hardware port (mkII standard)

## Default target

**NTS-1 mkII** — SDK **v2**, path `src/mkii/oscillators/<slug>/`, artifact `.nts1mkiiunit`, float `Osc::process()`.

Golden reference: `tr808_kick_phonk` — [docs/logue-mkii-golden-unit.md](../../docs/logue-mkii-golden-unit.md).

## Read first

- `docs/nts1-mkii-porting.md`
- `docs/logue-mkii-oscillator-agent.md`
- `.cursor/agents/logue-mkii-oscillator/SYSTEM.md`
- `.cursor/rules/03-build-targets.mdc`

## mkII workflow (primary)

1. Scaffold: `python3 tools/mkii/scaffold-mkii.py oscillators/<v1-folder>` (or new slug).
2. Implement `osc.h`: `process()`, `setParameter(index, 0..1023)`, note hooks.
3. Evaluate: `./tools/osc-eval-mkii.sh <slug>`.
4. Build: `./tools/build-mkii.sh <slug>` (`LOGUE_SDK` + `arm-none-eabi-gcc`).
5. Load: Kontrol Editor / loguecli per SDK `platform/nts-1_mkii/README.md`.

Bootstrap: `./tools/mkii-automate.sh bootstrap`

## v1.1 appendix (optional)

| Platform | Path | API |
|----------|------|-----|
| NTS-1 mkI | `nutekt-digital` | `OSC_*`, Q31 out |
| Minilogue XD / Prologue | `minilogue-xd` / `prologue` | Same binary class as mkI NTS |

- Hooks: `OSC_INIT`, `OSC_CYCLE`, `OSC_NOTEON`, `OSC_NOTEOFF`, `OSC_PARAM`
- Knobs: `manifest.json` + `OSC_PARAM` (not `params->p[]`)
- Build: `./tools/build.sh oscillators/<name> <platform>`
- See `.cursor/rules/01-sdk-api.mdc`

Port v1 → mkII: manual DSP in `osc.h` (scaffold does not auto-translate).

## Parameter mapping

- mkII knobs: **0–1023** in `setParameter`
- v1 presets often **0–100** → scale in `OSC_PARAM` or port helpers (`param_10bit_to_f32()`)

## Do not

- Use `osc_process`, `user_osc.c` drop-ins, or quarantined Claude bundle docs
- Include `userosc.h` in VST builds
- Include JUCE under `src/mkii/oscillators/`

## Deliverables

- mkII unit dir with `PORTING.md` updated
- `num_param` ≤ 10 (mkII)
- Pass `./tools/osc-eval-mkii.sh <slug>` before claiming complete
