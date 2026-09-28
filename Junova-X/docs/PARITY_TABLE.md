# Junova-X ↔ iPlug2 / reference parity (WO-2026-007 / 009)

**Ship tree:** `Junova-X/` (JUCE). **Import reference:** `vst-juno106/plugin/Juno106/` (external). **Open reference:** KR-106 (GPL, MIDI/SysEx only).

Status key: **Done** = implemented in Junova-X · **Partial** = MVP behavior, known gaps · **Pending** = blocked on iPlug2 tree or future WO.

## Import path (WO-2026-007)

```text
vst-juno106/third_party/iPlug2     ← submodule (framework)
vst-juno106/plugin/Juno106/        ← private product (not in git)
         │ extract param map + offline renders
         ▼
Junova-X/docs/PARITY_TABLE.md      ← this file
Junova-X/Source/DSP/*              ← JUCE implementation
tests/golden/junova/               ← Junova goldens + KR-106 compare
```

Commands:

```bash
git submodule update --init --depth 1 vst-juno106/third_party/iPlug2
# When Juno106/ exists on Windows:
bash vst-juno106/plugin/scripts/fetch-deps.sh
bash Junova-X/scripts/setup_kr106_reference.sh   # GPL vectors, not iPlug2
python3 Junova-X/scripts/make_kr106_golden_mid.py ab03-chorus-i /tmp/test.mid
```

## Subsystem parity

| Subsystem | Junova-X (JUCE) | KR-106 / literature | iPlug2 Juno106 | Gap / next |
|-----------|-----------------|---------------------|----------------|------------|
| **DCO** | Saw + PWM + sub + noise; drift param | J106 wave blend | _pending_ | Full Juno blend WO |
| **VCF** | SVF TPT + OTA tanh saturation | ngspice / IR3109 refs | _pending_ | Nonlinear IR3109 pass |
| **Chorus** | Dual BBD-style delay; LFO 0.393 / 0.797 / 8 Hz; Hermite | HW LFO rates | _pending_ | KR-106 A/B (`compare_kr106_reference.sh`) |
| **Env** | Dual ADSR (amp + filter) | Standard J106 | _pending_ | Velocity + cross-mod |
| **Arp** | Up; host PPQ 1/16–1/2; latch | — | _pending_ | Swing / patterns |
| **Voices** | 8 poly; unison; mono glide; **Juno 6** cap | 6 vs 106 modes | _pending_ | SysEx subset |
| **HPF** | Optional UI HPF | SW2 HPF nibble | _pending_ | — |
| **Presets** | 48 factory programs | 128 factory (KR-106) | _pending_ | Bank 2 |
| **IDs** | Mfr `SmFr`, code `JnvX` | — | Match handoff | Verify on import |

## Automated checks

| Check | Command |
|-------|---------|
| Junova goldens | `bash tests/golden/verify_junova_golden.sh` |
| KR-106 scenario MIDI | `make_kr106_golden_mid.py` + `compare_kr106_reference.sh` |
| Finish line | `bash Junova-X/scripts/finish_line.sh --mode full` |
| CI plugin QA | `python3 vst-testing-ops/run_business.py --profile ci-verify` |

## WO closure

- **WO-2026-007:** Parity table + import path documented; iPlug2 column fills when `plugin/Juno106/` lands.
- **WO-2026-009:** Chorus LFO + BBD v2 + OTA VCF documented in [ARCHITECTURE_DSP.md](ARCHITECTURE_DSP.md); KR-106 fixture for `ab03-chorus-i`.

## Related

- [IPLUG2_REFERENCE.md](IPLUG2_REFERENCE.md)
- [REFERENCE_PLUGINS.md](REFERENCE_PLUGINS.md)
- [QA_AB_JUN6.md](QA_AB_JUN6.md)
