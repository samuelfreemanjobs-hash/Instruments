---
name: apc-impl
description: APC Implement phase — scaffold vst/, write DSP/core logic, then editor. Only after Dream, Plan, Design.
---

# Implement phase (`/apc-impl [Name]`)

## Gate (mandatory)

```bash
bash tools/apc-gate.sh <Name> impl
```

Requires:

- `plugins/<Name>/.ideas/creative-brief.md`
- `plugins/<Name>/.ideas/parameter-spec.md`
- `plugins/<Name>/.ideas/architecture.md`
- `plugins/<Name>/.ideas/design-spec.md`

If gate fails, **stop** and tell user which phase to run.  
Exception: user explicitly says **skip to impl** (document waiver in `status.json`).

## Order of work

1. `./tools/new-vst.sh <Name> --type synth|effect` if `vst/<Name>/` missing (infer type from brief).
2. Sync `vst/<Name>/params.json` from `parameter-spec.md`.
3. **DSP / program chain first** — `shared/dsp/`, `Source/Dsp/`, `processBlock` per `architecture.md`.
4. **Surface chain** — APVTS + `PluginEditor` per `design-spec.md`.
5. Skills: `dsp-program-chain`, `dsp-parameter-mapping`, then `dsp-surface-chain`.

## status.json

- `vst_path`: `vst/<Name>`
- `validation.code_complete` = true
- `current_phase` = `implementation`

## Stop

Tell user: **`/apc-test <Name>`** then **`/apc-ship <Name>`** — implement is not “finished”.
