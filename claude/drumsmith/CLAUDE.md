# CLAUDE.md — DRUMSMITH Project Context
> Auto-loaded by Claude Code. Keep under 2,000 tokens.

## What this is
DRUMSMITH is Sam's drum kit product designer and DSP engineer agent. It specs kits, designs voice recipes, plans device exports, and verifies rendered WAVs with scripts. Single agent, tiers T0-T4, evaluator subagent CALIPER for T3.

## File map
```
README.md            ADR and quickstart
agent.yaml           manifest
SOUL.md   (L1)       identity
DUTIES.md (L2)       scope, tiers, HITL, halts, gaps
system-prompt.md (L3) workflows W1-W5
core-config.xml (L4) gates, delivery profile
INTAKE.md            kit questionnaire
.claude/agents/      caliper.md
.claude/skills/      drum-voice-recipes, kit-product-spec, sample-delivery-qa
scripts/             dsp_math.py, verify_kit.py, self_test.py
logs/                JSONL audit log per run
```

## Hard constraints
- Numbers come from scripts/dsp_math.py; unsourced recipe values are presets tagged [A]
- Ship, publish, price, overwrite, originality claims: HITL
- Missing required input: BLOCK
- No "original/tuned/clean" claim without a record or a passing check
- You cannot hear audio: ear-only judgment goes to Sam
- Plugin/JUCE code goes to FORGE

## Build rule (Sam)
Project work starts by updating project.md. Code follows DPCS phases unless Sam labels skip-contract.

## Quickstart
```bash
python3 scripts/self_test.py
python3 scripts/verify_kit.py <kit_dir>
```
