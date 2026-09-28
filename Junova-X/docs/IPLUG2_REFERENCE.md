# iPlug2 reference import (WO-2026-007)

**Ship target:** JUCE under `Junova-X/`.  
**Reference lane:** iPlug2 under `vst-juno106/` (import-only; no shared binary with JD Upgraded).

## In monorepo today

| Asset | Path | Status |
|-------|------|--------|
| iPlug2 framework | [`vst-juno106/third_party/iPlug2`](../../vst-juno106/third_party/iPlug2) | **Git submodule** (shallow) |
| Instrument template | `.../Examples/IPlugInstrument/` | Upstream example (not Juno DSP) |
| Disklordz Juno106 project | `vst-juno106/plugin/Juno106/` | **Not in git** — private / external scaffold per [REPO_HANDOFF](../../vst-juno106/REPO_HANDOFF.md) |
| JUCE product | `Junova-X/` | **Canonical ship tree** |

## Import checklist (WO-2026-007)

Import slot: [`vst-juno106/plugin/Juno106/README.md`](../../vst-juno106/plugin/Juno106/README.md).

When `plugin/Juno106/` lands (copy or submodule from private remote):

1. [ ] Verify `PLUG_UNIQUE_ID` / `PLUG_MFR_ID` match handoff (`JnvX` / `SmFr`) or document divergence.
2. [ ] Run [`vst-juno106/plugin/scripts/fetch-deps.sh`](../../vst-juno106/plugin/scripts/fetch-deps.sh) on **Windows x64** (VST3 SDK not committed).
3. [ ] Build **Release | x64** `Juno106-vst3`; attach pluginval log to WO.
4. [x] Extract **parameter map** (names, ranges, curves) → [PARITY_TABLE.md](PARITY_TABLE.md) + `ARCHITECTURE_DSP.md` (Junova column complete).
5. [x] For each row, record **Junova-X** vs reference — iPlug2 column _pending_ until `plugin/Juno106/` lands.
6. [x] Render matching MIDI — KR-106 SysEx fixtures: `Junova-X/fixtures/kr106_scenarios.json`, `make_kr106_golden_mid.py`, `compare_kr106_reference.sh`.
7. [x] WO-2026-007 **Done** (import path + parity doc); iPlug2 binary column updates when private tree available.

## Parity table (living)

Canonical: [PARITY_TABLE.md](PARITY_TABLE.md). Fill **iPlug2** column when reference code is available.

| Subsystem | Junova-X MVP | iPlug2 reference | JUCE next |
|-----------|--------------|------------------|-----------|
| DCO | saw/pwm/sub | _pending_ | full Juno wave blend |
| VCF | SVF ladder-ish | _pending_ | IR3109-style nonlinear |
| Chorus | dual delay BBD | _pending_ | clock-noise + stereo spread |
| Env | dual ADSR | _pending_ | cross-mod, velocity |
| Arp | up, PPQ, latch | _pending_ | swing, patterns |

## Submodule maintenance

```bash
git submodule update --init --depth 1 vst-juno106/third_party/iPlug2
```

Bump pointer intentionally after testing duplicate.py / example builds; do not vendor-edit iPlug2 core in this repo.

## Alternate open reference (JUCE, not iPlug2)

For DSP architecture ideas when iPlug2 Juno106 is blocked:

- [Ultramaster KR-106](https://github.com/kayrockscreenprinting/ultramaster_kr106) — Juno-60/106 modes, ngspice BBD, SysEx. **License: GPL** — do not copy code into Junova-X without explicit WO and license review; use for **test vectors and literature** only.

## Related

- [COMPETITIVE_JUN6.md](COMPETITIVE_JUN6.md) — A/B bibliography vs Arturia
- [junova-x-mvp-gap-analysis.md](junova-x-mvp-gap-analysis.md)
