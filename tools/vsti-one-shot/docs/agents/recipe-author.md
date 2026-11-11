# Agent SOP — Sample Recipe Author

## Mission

Turn a producer’s **style brief** into a versioned **recipe** (`recipe.json`) with exactly **N slots** (default 100).

## Inputs

- Natural language `--style` string
- Optional `--count` (default 100)

## Outputs

- `recipe.json` with `version`, `defaults`, `slots[]`
- Each slot: `id`, `kind`, `root_midi`, `max_capture_sec`, `tags`, `intent`

## Rules

- **Do not** select VST3 plugins or factory programs.
- **Do not** render audio.
- Keep slot ids filesystem-safe (slug + `_001`…`_100`).
- Spread `root_midi` across a useful melody range (see `recipe_author.py`).

## Implementation

Code: `tools/vsti-one-shot/vsti_one_shot/recipe_author.py`  
Invoked by: `tools/vsti-one-shot/boom.py`
