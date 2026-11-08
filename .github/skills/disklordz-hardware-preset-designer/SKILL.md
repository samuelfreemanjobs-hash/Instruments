---
name: disklordz-hardware-preset-designer
description: SynthForge workstation — batch hardware presets, prompt-to-patch, cloning, safety-clamped export.
when_to_use: User asks for hardware synth presets, SysEx/program batches, Minilogue/DX7/UltraNova patches, or SynthForge work.
disable-model-invocation: false
context: fork
paths:
  - "tools/synth-forge/**"
  - "disklordz/integrations/agents/hardware_preset_designer.py"
  - "disklordz/agents/profit/hardware-preset-designer/**"
  - "docs/DISKLORDZ*.md"
  - ".github/workflows/synth-forge.yml"
---

# Skill — Hardware Preset Designer

## When to use

- Batch preset generation, morphing, or lineage for **hardware synths**
- Natural-language **prompt-to-patch** (Cardo pad, phonk Reese, DX7 EP, etc.)
- WAV **sound cloning** into preset variations
- Librarian export staging / adapter hardening in SynthForge

## Prerequisites

- Read `tools/synth-forge/ARCHITECTURE.md` and `disklordz/agents/profit/hardware-preset-designer/agent.md`
- Run verification:

```bash
cd tools/synth-forge && python3 -m pytest tests -v
python3 disklordz/integrations/agents/hardware_preset_designer.py --self-test
```

## Success criteria

- [ ] `clamp_for_hardware` applied on all generated/exported params
- [ ] pytest or `--self-test` output attached
- [ ] Stub synths (`minifreak`, `zenology`) labeled if used
- [ ] No secrets in git

## Anti-patterns

- Bypassing safety clamps for “louder” patches
- Claiming bit-accurate librarian compatibility on stub adapters
- Editing SaaS Stripe/Supabase paths when task is hardware-only (stay in `tools/synth-forge/`)
