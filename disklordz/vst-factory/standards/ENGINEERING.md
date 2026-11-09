# VST Factory — engineering standards

Normative for **greenfield** plugins under `vst/<Name>/`. CHORD ENGINE follows [DiskLordz-Audio/](../../DiskLordz-Audio/) + [CHORD_ENGINE_PRD.md](../../docs/CHORD_ENGINE_PRD.md).

## Realtime (audio thread)

Follow [docs/dsp-architecture-specification.md](../../docs/dsp-architecture-specification.md):

- No heap allocation in `processBlock` / audio callback
- No locks, file I/O, network, or logging on the audio thread
- Parameter reads use smoothed values from the surface chain
- Denormals: flush or use FTZ/DAZ where supported

## Naming

| Item | Convention |
|------|------------|
| Plugin directory | PascalCase `vst/MeterTap/` |
| APVTS parameter IDs | `dl_<feature>_<name>` (e.g. `dl_mix_level`) |
| Manufacturer / plugin codes | Unique per SKU in `CMakeLists.txt` |
| Preset files | `Factory/<Category>/<Name>.xml` or project `Presets/` |

## Company defaults

Loaded from [memory/company.json](../memory/company.json) into every Cursor export.

## Git & repair

- Before automated repair batch: commit or tag working tree (operator policy)
- Max **3** repair attempts per project (`modules/workflows/scaffold-default.json`)
- Failed repair → human + `vst-testing-ops/error_log.txt`

## Acceptance (minimum ship bar)

1. `tools/build-vst.sh <Name>` succeeds on target OS
2. `python3 vst-testing-ops/run_business.py --profile factory --plugin <Name>`
3. Optional: `--profile factory-audio` when WAV fixture exists
4. `license-compliance` checklist before `release_ready: true` in spec
