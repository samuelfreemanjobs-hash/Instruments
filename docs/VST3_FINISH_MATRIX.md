# VST3 finish matrix

**Batch ship (implemented `vst/*`):** `./tools/finish-all-vst3.sh`

| Product | `vst/` | x86_64 | aarch64 Melodee | `release/` zip |
|---------|--------|--------|-----------------|----------------|
| MpcAnalogSynth | yes | batch | batch | batch |
| MpcCs80 | yes | batch | batch | batch |
| MpcDxFm | yes | batch | batch | batch |
| MpcJuno106 | yes | batch | batch | batch |
| MpcMessenger | yes | batch | batch | batch |
| MpcMinimoog | yes | batch | batch | batch |
| MpcSh101 | yes | batch | batch | batch |
| MpcSubPhatty | yes | batch | batch | batch |
| MpcTr808909 | yes | batch | batch | batch |
| MpcVoyager | yes | batch | batch | batch |
| MeterTap | yes | batch | — | batch |
| MemphisPhonkFactory | yes | batch | — | batch |

**APC only (not VST3 yet):** Nebula6, Play 6-pack (`PhonkCowbell94` …), MpcProphet5, MpcObXa, MpcPolysix, MpcLaSynth, SemV.

**aarch64 (Melodee):** `./tools/finish-all-vst3.sh` with `SKIP_AARCH64=0` — requires `libasound2-dev:arm64` + `libfreetype6-dev:arm64` (multiarch; cloud image may lack arm64 apt indexes).

**Next desktop phonk:** `/apc-impl Nebula6` (design complete); `run-factory-play6.sh --skip-claude` for Play 6-pack.