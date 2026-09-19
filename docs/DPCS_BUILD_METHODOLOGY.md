# DPCS — Build Methodology

Every module in this repo was built with the same phase-locked pipeline.
Continue it.

```
PHASE 0  Constitution   scope, stack, non-negotiables, exclusions
PHASE 1  PRD            behaviour only — ZERO code, no imports or frameworks
PHASE 2  Contract       headers only, compile-checked, no implementation
PHASE 3  Module         ONE .cpp per run, explicit file allowlist
PHASE 4  Verify         a script that exits 0, or it isn't done
```

## Core laws

- **DONE = verifiable commands exit 0.** Not an opinion that something looks
  good.
- **Zero code in Phase 1.** Behaviour only.
- **One module per Phase 3 run.** No refactors outside the allowlist, no
  invented dependencies, no whole-app dumps.
- **Upstream by path.** Cite artifact paths; never rely on chat memory.

## Debug protocol

Order matters. Do not skip ahead to the patch.

1. Observable facts
2. At most 3 hypotheses
3. **Root cause proof paragraph — written before any fix**
4. Minimal fix
5. Regression test

This protocol is why the corrections in `docs/SIGNAL_CHAIN.md` are specific
rather than vague. Two examples:

**C8** — the processing flow specified one resampler push per output sample.
The proof: `phaseIncrement = kZOHRate / hostSampleRate` is 2.17 at 48 kHz, never
1.0, so the flow would consume ZOH samples at 46% of the required rate. Shipped
literally, every voice would have played a fifth of an octave low. Found by
computing the ratio, not by listening.

**MergeEvents** — 29 tests passed. Mutation testing survived one mutation, so
rather than assume it was equivalent, the behaviour was measured: a batch of
5,000 events with 3 in range was refused despite trivially fitting, because the
capacity check counted raw batch size instead of storable events. That would
have made the recorder retry forever and silently lose material after a
truncation.

## Verification techniques used here

**Mutation testing.** Break a mechanism on purpose, confirm the suite catches
it. A suite that passes against a broken implementation is not a suite.

**Equivalent-mutant analysis.** When a mutation survives, measure whether it
actually changes behaviour before writing a test. Two survivors in
`SwingTransform` were proven equivalent by exhaustive check — an unreachable
clamp and a bit-identical optimization. Writing tests for those would have been
writing assertions that cannot fail. Both are documented in-source instead, so
nobody later mistakes defensive code for load-bearing code.

**Sanitizers as a distinct class of coverage.** Logic assertions cannot observe
use-after-free, leaks, or data races. ASan found C12 (a full leak of the sample
library) when every logic test passed. TSan covers the lock-free buffer swap in
`SegmentStore`.
