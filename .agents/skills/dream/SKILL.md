---
name: apc-dream
description: APC Dream phase — creative brief, parameter spec, reference audio (Noizefield Part 2).
---

# Dream phase (`/apc-dream [Name]`)

## Output restrictions

- **No C++**, no CMake, no `vst/` code in this phase.

## Step 1 — Interview (if prompt is vague)

Ask up to 3 questions:

1. Sonic goal / character?
2. Top parameters (3–8)?
3. Visual vibe?

Wait for answers unless user already gave a full brief (Part 2 style).

## Step 2 — Workspace

```bash
bin/apc init <Name>   # if plugins/<Name> does not exist
```

Ask user to drop reference WAVs in `plugins/<Name>/references/` (turntable noise, etc.).

If audio present, write `.ideas/reference-analysis.md` (spectral character, noise floor, dynamics — text analysis; no ML required).

## Step 3 — Files

From `templates/apc/`:

- `.ideas/creative-brief.md`
- `.ideas/parameter-spec.md` (table of all controls)

Update `status.json`:

- `validation.creative_brief_exists` = true
- `validation.parameter_spec_exists` = true
- `current_phase` = `ideation`

## Step 4 — Stop

Tell user the mandatory next step only: **`/apc-plan <Name>`**  
Do not offer implement or ship in the same turn.
