# SOP-VF-13 — Visual regression baselines (factory GUI)

## Purpose

Catch UI bugs that pixel-colour unit checks miss: truncated readouts (`...`), mojibake (`METERS Â· INPUT`), wrong meter hot colour across the whole peak bar, clipped header text, wrong LED-on colour. Render tests write deterministic PNGs + text sidecars; `tools/visual_diff.py` compares them to committed goldens and fails CI when they diverge.

## Golden cell

| Field | Value |
|-------|-------|
| Suite | `tests/visual/golden/factory-gui/` |
| OS | `ubuntu-latest` (GitHub Actions runner — **sole author**) |
| JUCE | **8.0.15** (design-system workflow matrix cell) |
| Scales | `1x`, `2x` |
| Themes | every theme under `gui/generated/` (`disklordz`, `_template`, `studio-light`) |
| Manifest | `tests/visual/golden/factory-gui/MANIFEST.json` (JUCE tag, OS, font SHA-256s) |
| Tolerances | **Strict:** `--channel-tol 8` · `--max-diff-ratio 0.002` (0.2%). Text sidecars exact. Do **not** loosen for host AA. |

Comparing against a different JUCE tag or changed font bytes fails closed until you re-baseline consciously.

## CI-authored goldens (mandatory)

**Never commit goldens rendered on a developer machine, Cursor Cloud VM, or any host other than the design-system `ubuntu-latest` JUCE 8.0.15 cell.**

Environment mismatch (FreeType / fontconfig / GPU vs software) is not a reason to widen tolerances. Loosening to ~8–10% of pixels lets real regressions (LED colour, focus ring, hold line, over cap) pass. Instead:

1. Land the intentional UI change on the PR branch.
2. Trigger a CI re-baseline on that branch:

```bash
gh workflow run design-system.yml --ref <branch> -f rebaseline=true
# optional selective: -f only='HeaderBar*,PresetBrowser*'
```

3. The golden cell runs `tools/visual_diff.py --update` (writes `REVIEW.md` + `_review/` thumbs), commits as `github-actions[bot]` with message `visual: re-baseline on CI runner (<run url>)`, and uploads the golden tree as artifact `factory-gui-goldens-rebaseline`.
4. Pull the bot commit; open `REVIEW.md` before merge. A following normal design-system run must pass at the strict tolerances against those CI-authored goldens.

## How to add a component to visual tests

1. In `vst/factory-lib/gui/tests/FactoryGuiRenderTests.cpp`, add a `capture…` helper that:
   - Constructs the component with LookAndFeel bound.
   - Sets each required **state** (default / hover / focus / disabled; meters: silence / normal / hot / over-latched; readouts: neg-inf / neg96 / plus12 / over; headers: longest product name).
   - Calls `capture(component, "ComponentName", "state", scale)` so both `.png` and `.json` are written under `build/…/gui-renders/<theme>/`.
2. Size boxes from the **longest** value the control will show (e.g. `−96.0`, `+12.0`). Never ship a width that forces ellipsis.
3. Non-ASCII UI strings must go through `fds::gui::utf8()` / `middot()` / `minusSign()` (never bare `"·"`).
4. Rebuild + run render tests, then re-baseline **via CI** (above) in the same PR as the new captures.
5. Prefer pinning HeaderBar longest product text (`CHORD ENGINE`) so host font metrics do not introduce flake.

## How to review a diff report

CI uploads `visual-diff-report/` on failure (HTML + per-image `*.diff.png`).

1. Open `report.html`.
2. Each failing row shows **golden | actual | highlighted diff**.
3. Ask: intentional design change, or regression?
   - Intentional → trigger CI re-baseline; skim every changed PNG in `REVIEW.md`.
   - Regression → reject; fix the component / layout; do **not** `--update` locally and push.
4. Text sidecar failures (string mismatch, ellipsis, bounds outside component) are never “AA noise” — treat as bugs.

## When / how to re-baseline

**Preferred (required for merge):** CI `workflow_dispatch` with `rebaseline=true` (see above).

Local `--update` is for inspecting REVIEW.md only — do **not** commit those PNGs:

```bash
# After FactoryGuiRenderTests wrote build-factory-gui-8.0.15/gui-renders/
# (inspection only — discard; CI must author the committed goldens)
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
- Selective re-baseline: `--only 'HeaderBar*'` (repeatable) or workflow input `only=HeaderBar*,PresetBrowser*`. Stop if unexpected components also differ.
- Update `MANIFEST.json` travels with `--update` (JUCE tag + font hashes).
- Never re-baseline to silence a failure you have not inspected (L-0005).
- Do **not** bake known bugs into goldens while a fix PR is still open — wait for merge, then re-baseline only the fixed surfaces.

## MeterTap editor goldens (follow-up)

Product editor goldens for MeterTap are **out of scope** for this suite. Add them under `tests/visual/golden/metertap-editor/` in a follow-up PR — do not block factory-gui baselines on that work.

## Related

- [SOP-VF-06 Verify](SOP-VF-06-verify-test.md) — Visual regression section
- [design-system ARCHITECTURE](../../../vst/factory-lib/design-system/ARCHITECTURE.md)
- [DPCS.md](../../DPCS.md) — mutation testing; DONE = commands exit 0
- Agent memory: L-0001 (delete before PNG write), L-0003 (UTF-8), L-0005 (inspect artefacts), L-0012 (assert rendered text)
