# DRUMSMITH architecture (v1.3 in-repo)

## Purpose

Verifiable drum kit production for Disklordz: kit spec → genre/recipe synthesis → render → `verify_kit.py` → diversity audit → FORGE JSON handoff. CALIPER is read-only QA. Commercial release requires human listening + named approval.

## Build & run

From `claude/drumsmith/`:

```bash
python3 scripts/self_test.py      # 28
python3 scripts/test_upgrade.py   # v1.2 recipe engine
python3 scripts/test_v13.py       # genre factory

python3 scripts/factory.py --out /tmp/my_phonk_kit --genre drift-phonk --sku DKL --count 64
python3 scripts/kit_browser.py --kit /tmp/my_phonk_kit   # http://127.0.0.1:8765
python3 scripts/verify_kit.py --kit examples/tail-light-static/tls-v13-drift \
  --manifest examples/tail-light-static/tls-v13-drift/manifest.json
```

## Data flow

```text
INTAKE / PRODUCT_SPEC → factory/genre recipes → render WAVs → verify_kit + pitch_check
  → diversity_audit → export_forge.json → (FORGE / JUCE later)
```

## Key modules

| Path | Role |
|------|------|
| `scripts/factory.py` / `genre_profiles.py` | v1.3 genre kit build |
| `scripts/recipe_engine.py` / `build_recipe_kit.py` | v1.2 parametric recipes + fingerprints |
| `scripts/render_*.py` | Offline reference synthesis (not commercial masters) |
| `scripts/verify_kit.py` / `pitch_check.py` / `diversity_audit.py` | Technical QA |
| `scripts/kit_browser.py` | Local audition UI |
| `scripts/export_forge.py` | JSON voice specs for FORGE |
| `examples/tail-light-static/` | TLS demos (v1.1 reference, v1.2 recipes, v1.3 drift) |
| `workbench/` | Single-file UI template (artifact parity) |
| `kits/tail-light-static/` | Monorepo W1 intake/spec notes |

## Related docs

- `project.md` — Rule #1 status
- `docs/WORKFLOW_V13.md`, `docs/RELEASE_GATE.md`
- Root `ARCHITECTURE.md` index
