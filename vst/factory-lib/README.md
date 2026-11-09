# DiskLordz VST Factory — reusable C++ library (Phase 4)

Stubs referenced by `disklordz/vst-factory/modules/dsp/*.json` and `modules/gui/controls.json`.

Implement blocks here; APC scaffolds import via CMake `target_include_directories`.

Do not duplicate `shared/dsp/` without merging conventions from [docs/dsp-architecture-specification.md](../../docs/dsp-architecture-specification.md).

## UI: DiskLordz Audio Design System

All `gui/` components follow [design-system/](design-system/ARCHITECTURE.md): tokens (`tokens.json`), [foundations](design-system/foundations.md), [component specs](design-system/components.md) and a generated [visual preview](design-system/preview.html). Check with `python3 vst/factory-lib/design-system/scripts/check_tokens.py --check-preview`.
