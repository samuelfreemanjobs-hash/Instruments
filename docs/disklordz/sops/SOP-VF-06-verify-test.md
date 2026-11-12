# SOP-VF-06 — Verify, test & certify (DPCS Phase 4)

## Purpose

Prove the product meets its spec with commands that exit 0, then let the factory certify it.

## Procedure

1. **Verify script:** `bin/apc template <Name> verify` → `plugins/<Name>/scripts/verify-phase4.sh`. Add every check from architecture §7 (V1…Vn) as a step.
2. **Unit tests** for JUCE-free blocks: sine/silence/NaN/sample-rate/block-size cases with tolerances from the brief; drift or long-run tests where state accumulates.
3. **Processor tests:** bypass bit-identity, parameter laws after settle, non-finite input → finite output.
4. **Mutation check:** break one mechanism on purpose (skip a recompute, remove a clamp), confirm a test fails, restore. A suite that passes against broken code is not a suite. If a mutation survives, measure whether behaviour actually changed before adding a test (equivalent mutant).
5. **Build:** `tools/build-vst.sh <Name>` (Linux CI; Windows per [SOP-VF-12](SOP-VF-12-windows-operator.md)).
6. **Factory QA:** `python3 vst-testing-ops/run_business.py --profile factory --plugin <Name>`; on failure read `vst-testing-ops/error_log.txt`.
7. **Re-theme check** (if the editor uses the design system): build once with another theme; it must compile and render.
8. **pluginval + DAW smoke:** on the Windows operator machine when online (tier A; not a CI blocker).
9. **Factory:** `POST /api/projects/{id}/test` then `/certify`; certification fails closed if a required run is missing.
10. **Status:** `tests_passed` true, `dpcs_phase` 4, `current_phase` `test`.

## Definition of done

```bash
bash plugins/<Name>/scripts/verify-phase4.sh            # exit 0
# factory: certify -> {"passed": true}
```

## Golden files

Intentional DSP output changes: `tests/golden/refresh_golden.sh`, commit the WAVs, explain in the PR.

## Visual regression

Factory GUI components must pass image + text golden comparison before certify:

```bash
# After FactoryGuiRenderTests (JUCE 8.0.15 golden cell)
python3 tools/visual_diff.py \
  --actual build-factory-gui-8.0.15/gui-renders \
  --golden tests/visual/golden/factory-gui \
  --out build/visual-diff \
  --expected-juce 8.0.15
# exit 0 required; on failure open build/visual-diff/report.html
```

- PNGs + text sidecars come from `FactoryGuiRenderTests` (`gui-renders/<theme>/<component>-<state>@<scale>x.{png,json}`).
- Unit tests for the differ: `python3 -m unittest discover -s tools -p 'test_visual_diff.py'`.
- Intentional UI changes: re-baseline **on CI** (`gh workflow run design-system.yml --ref <branch> -f rebaseline=true`); never commit goldens from a dev/agent VM ([SOP-VF-13](SOP-VF-13-visual-baselines.md)). Reviewer opens `REVIEW.md` + the HTML report.
- Tolerances stay strict (`channel-tol 8`, `max-diff-ratio 0.2%`). Do not loosen for FreeType host mismatch.
- MeterTap **editor** goldens are a follow-up — not part of the factory-gui suite yet.

## Related

[DPCS.md](../../DPCS.md) (debug protocol, mutation testing) · [vst-testing-ops/ARCHITECTURE.md](../../../vst-testing-ops/ARCHITECTURE.md) · [SOP-VF-09 Debug](SOP-VF-09-debug-repair.md) · [SOP-VF-13 Visual baselines](SOP-VF-13-visual-baselines.md)
