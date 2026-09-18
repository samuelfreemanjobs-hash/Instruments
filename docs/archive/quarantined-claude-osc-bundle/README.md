# Quarantined: external “Korg Oscillator Designer” Claude bundle

**Status:** Quarantined — **do not use for code generation** in this repository.

## Why

Third-party agent package (master prompt + skills + `example-morphing-sawtooth.c` + generic Makefile BUILD guide) was reviewed and found to:

- Use **non-existent APIs** (`osc_init`, `osc_process`, `params->p[]`)
- Target **wrong CPUs** (M7 XD vs M0+ NTS) for logue **v1.1**
- Ship an **example .c** that does not match `userosc.h` and is not compilable as pasted
- Conflict with **mkII-first** standard (SDK v2, `Osc::process`, `.nts1mkiiunit`)

## Use instead

| Need | Path |
|------|------|
| mkII agent prompt | `.cursor/agents/logue-mkii-oscillator/SYSTEM.md` |
| Workflow | `docs/logue-mkii-oscillator-agent.md` |
| API v1.1 summary | `.cursor/rules/01-sdk-api.mdc` |
| mkII porting | `docs/nts1-mkii-porting.md` |
| Golden code | `src/mkii/oscillators/tr808_kick_phonk/` |

## If reviving this bundle

Requirements before un-quarantine:

1. Rewrite all skills **mkII-first**; v1.1 as appendix only
2. Replace example with a verified unit under `src/mkii/oscillators/`
3. BUILD guide must reference `tools/build-mkii.sh` / `mkii-automate.sh`
4. Pass `./tools/osc-eval-mkii.sh` on the reference slug

Original files were never committed to git; this README records the decision only.
