# Night Circuit — host compatibility matrix

**Format:** Desktop **VST3** only (`Night Circuit.vst3`).  
**Not in scope:** MPC **standalone device** as VST3 host (use MPC **Software** on desktop).

## What Cursor VM proves vs does not prove

| Activity | Cursor Linux VM |
|----------|-----------------|
| CMake build, `NightCircuitTests`, `verify_preset_bank.py` | Valid |
| pluginval VST3 smoke | Valid |
| Offline render scripts (future) | Valid |
| FL Studio load, MPC Software load | **Not valid — use target OS** |
| Audio device latency, driver glitches | **Not valid** |
| Windows/macOS HiDPI editor layout | **Not valid** |

Record VM results under **CI / automated**; record DAW results only from real host installs.

## Primary targets (product)

Test and sign off with **exact version strings** (edit this table when tested):

| Host | OS | Version tested | Plugin scan | Editor open | MIDI in | Preset recall | Project save/reload | Tester | Date | Result |
|------|-----|----------------|-------------|-------------|---------|---------------|---------------------|--------|------|--------|
| **FL Studio** | Windows 10/11 | _TBD_ | | | | | | | | NOT TESTED |
| **FL Studio** | macOS | _TBD_ | | | | | | | | NOT TESTED |
| **MPC Software** | Windows 10/11 | _TBD_ | | | | | | | | NOT TESTED |
| **MPC Software** | macOS | _TBD_ | | | | | | | | NOT TESTED |

## Secondary / CI

| Host | OS | Notes |
|------|-----|--------|
| JUCE Standalone | Linux | Dev smoke only; same DSP as VST3 |
| pluginval | Linux | Automation; not a musical host |

## Failure reporting

For each failure: host version, OS build, steps, expected vs actual, `.vst3` path, git sha. File under `ProphetRev2Trap/qa/reports/` or `design/reports/` and link from QA release recommendation.
