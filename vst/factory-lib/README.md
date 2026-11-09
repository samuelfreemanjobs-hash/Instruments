# DiskLordz VST Factory — reusable C++ library (Phase 4)

Stubs referenced by `disklordz/vst-factory/modules/dsp/*.json` and `modules/gui/controls.json`.

Implement blocks here; APC scaffolds import via CMake `target_include_directories`.

Do not duplicate `shared/dsp/` without merging conventions from [docs/dsp-architecture-specification.md](../../docs/dsp-architecture-specification.md).

## UI: themeable design system

All `gui/` components follow [design-system/](design-system/ARCHITECTURE.md): a role [contract](design-system/contract/roles.json), swappable themes (`themes/disklordz`, `themes/_template`, `themes/studio-light`), [foundations](design-system/foundations.md), [component specs](design-system/components.md) and generated previews (`design-system/previews/<theme>.html`). Check with `python3 vst/factory-lib/design-system/scripts/check_tokens.py --check-preview`.
