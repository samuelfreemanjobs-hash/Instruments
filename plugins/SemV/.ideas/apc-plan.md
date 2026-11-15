# SemV — APC plan

**Architecture:** [architecture.md](architecture.md) · **Next:** `/apc-design SemV`

**Chosen Arturia clone (1 of 1):** **SEM V** (Oberheim SEM) → product **`SemV`**.

## Complexity (spec §8)

| Dimension | Score | Notes |
|-----------|-------|-------|
| Voices | 2 | 8-voice fixed pool; mono legato mode |
| Oscillators/voice | 2 | Dual osc + sub + noise |
| Filters | 2 | SVF multimode (LP/BP/HP/notch) |
| Modulation | 1 | 1 LFO → one dest; dual ADSR |
| FX | 2 | Chorus + delay + light reverb post-sum |
| Surfaces | 2 | Native JUCE editor + 12 factory presets |
| Targets | 1 | Desktop VST3 x86_64/Linux (macOS/Win when available) |
| **Total** | **12** | |

**Tier:** **L2** (Advanced)

**Risk flags:**

- SVF stability at high resonance — test at 44.1/48 kHz; optional gentle output clip post-filter.
- FX delay feedback < 0.85 to avoid runaway; cap delay line at 2 s allocated at `prepare`.
- No oversampling v1; revisit if PWM aliasing audible on bright leads.

## UI framework

| Field | Value |
|-------|-------|
| `ui_framework` | `juce-native` |
| Decision | Knob section UI (Arturia SEM panel density, not WebView Play shell) |
| Rationale | SEM is a **traditional synth UI**; faster to ship than HTML; matches `docs/JUCE8_VST3_PROJECT_RULES.md` |

## Implementation strategy

1. **Gate:** `./tools/apc-gate.sh SemV impl` after design.
2. Scaffold: `./tools/new-vst.sh SemV --type synth` → `vst/SemV/` (post-design only).
3. Implement **program chain first** (`SemVoice` + SVF + osc), silent editor smoke, then FX bus, then editor layout.
4. Reuse patterns from `ProphetRev2Trap` / `vst/MpcAnalogSynth` for voice pool + `SmoothParam`; do **not** fork Juno chorus code blindly — SEM uses SVF + different FX voicing.
5. Factory: 12 presets from `parameter-spec.md`; optional queue row in `overnight-queue.json` later.

## Alternatives considered (not selected)

| Arturia title | Why skipped for this pick |
|---------------|---------------------------|
| Jun-6 V | Overlaps `MpcJuno106` / Play 80s lane |
| Mini V | Overlaps `MpcMinimoog` |
| Jup-8 V | L3 scope for v1 |
| Buchla Easel V | Different West Coast paradigm — good **second** SKU |