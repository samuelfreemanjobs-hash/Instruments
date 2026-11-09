# vst-factory-director — incidents (dated problems and fixes)

Format and rules: [MEMORY.md](MEMORY.md).

### I-0001 · Factory server could not start: modules never committed
- **date:** 2026-11-08
- **area:** factory, python
- **when:** starting uvicorn on cursor/vst-factory-foundation-f2dd
- **lesson:** vst_factory.certification and vst_factory.portfolio were imported but existed on no branch; the export bundle that may have held them was only on a Cloud VM.
- **do:** Rebuilt by a Cloud agent from call sites and tests (PR #121, 17 tests, MeterTap smoke PASS); merged to main via PR #120.
- **evidence:** ModuleNotFoundError at import; PR #121 smoke output
- **confidence:** executed
- **status:** active

### I-0002 · Design-system C++ failed CI on the pinned JUCE
- **date:** 2026-11-09
- **area:** juce, ci
- **when:** PR #126 tokens-and-render
- **lesson:** `juce::FontOptions` is not in JUCE 7.0.12; local build used a cached JUCE 8.0.15 tree.
- **do:** Dual-version font helper, CI matrix (from-file + 8.0.15), no silent build/_deps reuse (see L-0002).
- **evidence:** run 42571508857; fix 251f4fb8 + 56162c76
- **confidence:** executed
- **status:** active

### I-0003 · MeterTap 1.0.0 editor review found 5 UI bugs and corrupt evidence
- **date:** 2026-11-09
- **area:** ui, review, juce
- **when:** reviewing PR #127 renders
- **lesson:** Concatenated PNGs (FileOutputStream append), peak bar fully hot, UTF-8 mojibake, empty RMS readouts, overlapping bypass dot; all tests had passed.
- **do:** Fixes routed: LevelMeter + UTF-8 to the design-system agent, editor + evidence writer to the MeterTap agent; each fix requires a failing-first test (see L-0001, L-0003, L-0005).
- **evidence:** PNG structure check (5 IEND per file); render screenshots
- **confidence:** executed
- **status:** active
