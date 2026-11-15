# SOP-VF-09 — Debug & repair

## Purpose

Fix failures (build errors, silence, clicks, wrong values, crashes, CI red) with proof before patch.

## Procedure (DPCS debug protocol; order matters)

1. **Observable facts:** exact error text, failing command, platform, sample rate, block size, inputs. Reproduce first; if you can't reproduce, say so.
2. **At most three hypotheses**, each with a quick test that would falsify it.
3. **Root-cause proof paragraph, written before any fix:** the mechanism, with numbers where possible (e.g. a ratio that should be 1.0 but is 2.17).
4. **Minimal fix** inside the failing module's allowlist.
5. **Regression test** that fails on the old code and passes on the new.
6. Record it in the PR: facts → proof → fix → test.

## Common cases

| Symptom | First look |
|---------|-----------|
| `ModuleNotFoundError` in factory | file never committed: `git log --all -- <path>`; restore or reimplement on a branch |
| Gate refuses (`apc-gate.sh`) | file path vs expected (`.ideas/…`); fix the location, update links |
| Silence | bypass state, bus layout rejected, gain smoothing reset to 0 |
| Clicks / zipper | parameter not smoothed, or smoothing reset in `prepareToPlay` with a ramp |
| NaN at output | missing sanitize, division by zero, denormals |
| Windows build slow / huge | building whole root project; build one target (SOP-VF-12) |

## Related

[docs/DPCS.md](../../DPCS.md) · [.agents/skills/debug/SKILL.md](../../../.agents/skills/debug/SKILL.md) · [TEMPLATE-incident-debug.md](../templates/TEMPLATE-incident-debug.md)
