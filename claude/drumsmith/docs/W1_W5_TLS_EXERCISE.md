# W1–W5 exercise — Tail Light Static native (`tls-v20-native-cxx`)

**Date:** 2026-11-17 · **Phase:** agent-build · **Kit:** `examples/tail-light-static/tls-v20-native-cxx`

## W1 SPEC

| Item | Status |
|------|--------|
| INTAKE / kit-spec | Present under `kits/tail-light-static/` + example PRODUCT notes |
| Promise | Drift-phonk one-shots; agent-demo |
| Devices | UNVERIFIED |

No new commercial claims.

## W2 BUILD

Pre-rendered native masters (64 WAV) already in tree from v2.0 merge. Re-build path documented in `project.md` (`g++-12` + `native_factory.py`).

## W3 VERIFY [executed]

```bash
python3 scripts/verify_kit.py --kit examples/tail-light-static/tls-v20-native-cxx \
  --manifest examples/tail-light-static/tls-v20-native-cxx/manifest.json \
  --json examples/tail-light-static/tls-v20-native-cxx/qa-report.json
python3 scripts/diversity_audit.py --kit examples/tail-light-static/tls-v20-native-cxx \
  --json examples/tail-light-static/tls-v20-native-cxx/diversity-report.json
```

| Check | Result |
|-------|--------|
| Technical QA | PASS · `release_ready=false` |
| Warnings | listening not approved; release not approved; devices unverified |
| Diversity | PASS · 0 exact dupes · advisory similar-envelope pairs (human review) |
| CALIPER | See `examples/.../docs/CALIPER_AUDIT.md` → **BLOCKED** for commercial release |

## Loop / groove tooling [executed]

```bash
python3 scripts/trap_loop_architect.py --name tls-2bar --genre drift-phonk --bpm 74 --bars 2 \
  --out examples/tail-light-static/tls-v20-native-cxx/docs/tls-2bar-loop.json \
  --midi-out examples/tail-light-static/tls-v20-native-cxx/docs/tls-2bar-loop.mid
python3 scripts/learn_midi_groove.py --midi .../tls-2bar-loop.mid --out .../tls-2bar-groove.json
```

`samples_per_bar` = 143027 [C] via `loop_samples(1,74,44100)`.

## W4 PACKAGE

**Not opened.** Agent-demo only. No storefront zip.

## W5 IMPROVE / workbench

Listening notes stub: `examples/.../docs/listening-notes.md` (awaiting human).  
Procedure: `docs/WORKBENCH_LOOP.md` · open `workbench/DRUMSMITH_Workbench.html` or `kit_browser.py`.

## Stalls observed

1. Human listening gate blocks W4/W5 closure (expected).
2. Advisory envelope similarity needs ear — scripts cannot clear.
3. CALIPER must stay a separate pass (`docs/CALIPER_CURSOR.md`), not self-check.
