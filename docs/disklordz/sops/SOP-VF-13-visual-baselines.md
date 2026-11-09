# SOP-VF-13 — Visual regression baselines (factory GUI)

## Purpose

Catch UI bugs that pixel-colour unit checks miss: truncated readouts (`...`), mojibake (`METERS Â· INPUT`), wrong meter hot colour across the whole peak bar, clipped header text. Render tests write deterministic PNGs + text sidecars; `tools/visual_diff.py` compares them to committed goldens and fails CI when they diverge.

## Golden cell

| Field | Value |
|-------|-------|
| Suite | `tests/visual/golden/factory-gui/` |
| OS | `ubuntu-latest` (Linux) |
| JUCE | **8.0.15** (design-system workflow matrix cell) |
| Scales | `1x`, `2x` |
| Themes | every theme under `gui/generated/` (`disklordz`, `_template`, `studio-light`) |
| Manifest | `tests/visual/golden/factory-gui/MANIFEST.json` (JUCE tag, OS, font SHA-256s) |
| CI AA | design-system golden cell uses `--channel-tol 24 --max-diff-ratio 0.10` for FreeType host variance; tool defaults stay `8` / `0.2%` for local mutation proofs. Text sidecars remain exact. Whole-bar-hot mutations still land ~20%+. |

Comparing against a different JUCE tag or changed font bytes fails closed until you re-baseline consciously. Prefer regenerating PNGs from the CI `factory-gui-renders-juce-8.0.15` artifact with `--update` (writes `REVIEW.md`) when host AA drifts beyond the CI allowance.

## How to add a component to visual tests

1. In `vst/factory-lib/gui/tests/FactoryGuiRenderTests.cpp`, add a `capture…` helper that:
   - Constructs the component with LookAndFeel bound.
   - Sets each required **state** (default / hover / focus / disabled; meters: silence / normal / hot / over-latched; readouts: neg-inf / neg96 / plus12 / over; headers: longest product name).
   - Calls `capture(component, "ComponentName", "state", scale)` so both `.png` and `.json` are written under `build/…/gui-renders/<theme>/`.
2. Size boxes from the **longest** value the control will show (e.g. `−96.0`, `+12.0`). Never ship a width that forces ellipsis.
3. Non-ASCII UI strings must go through `fds::gui::utf8()` / `middot()` / `minusSign()` (never bare `"·"`).
4. Rebuild + run render tests, then re-baseline (below) **in the same PR** as the new captures.
5. Confirm `python3 tools/visual_diff.py …` exits 0 locally before pushing.

## How to review a diff report

CI uploads `visual-diff-report/` on failure (HTML + per-image `*.diff.png`).

1. Open `report.html`.
2. Each failing row shows **golden | actual | highlighted diff**.
3. Ask: intentional design change, or regression?
   - Intentional → approve re-baseline in this PR; skim every changed PNG.
   - Regression → reject; fix the component / layout; do **not** `--update`.
4. Text sidecar failures (string mismatch, ellipsis, bounds outside component) are never “AA noise” — treat as bugs.

## When / how to re-baseline

Re-baseline **only** when the UI change is intentional, and **only** with the explicit flag:

```bash
# After FactoryGuiRenderTests wrote build-factory-gui-8.0.15/gui-renders/
python3 tools/visual_diff.py \
  --actual build-factory-gui-8.0.15/gui-renders \
  --golden tests/visual/golden/factory-gui \
  --out build/visual-diff \
  --expected-juce 8.0.15 \
  --update
```

Rules:

- Same PR as the intentional UI change.
- Reviewer **must** open `REVIEW.md` (written next to `MANIFEST.json` by `--update`) and the HTML diff report before merge. `REVIEW.md` lists every changed golden with before/after thumbnail links under `_review/`.
- Selective re-baseline: `--only 'HeaderBar*' --only 'PresetBrowser*' --only 'ValueReadout*'` (repeatable globs). Stop if unexpected components also differ.
- Update `MANIFEST.json` travels with `--update` (JUCE tag + font hashes).
- Never re-baseline to silence a failure you have not inspected (L-0005).
- Do **not** bake known bugs into goldens while a fix PR is still open — wait for merge, then re-baseline only the fixed surfaces.

## MeterTap editor goldens (follow-up)

Product editor goldens for MeterTap are **out of scope** for this suite. Add them after [PR #127](https://github.com/samuelfreemanjobs-hash/Instruments/pull/127) merges (editor fixes for peak/hot, UTF-8, readout width, header clipping). Track under `tests/visual/golden/metertap-editor/` in a follow-up PR — do not block factory-gui baselines on that work.

## Related

- [SOP-VF-06 Verify](SOP-VF-06-verify-test.md) — Visual regression section
- [design-system ARCHITECTURE](../../../vst/factory-lib/design-system/ARCHITECTURE.md)
- [DPCS.md](../../DPCS.md) — mutation testing; DONE = commands exit 0
- Agent memory: L-0001 (delete before PNG write), L-0003 (UTF-8), L-0005 (inspect artefacts), L-0012 (assert rendered text)
