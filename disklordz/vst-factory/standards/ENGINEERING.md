# VST Factory — engineering standard

## Scope

Rules for greenfield APC plugins under `plugins/<Name>/` + `vst/<Name>/` orchestrated by DiskLordz VST Factory.

## Non-negotiables

1. Follow APC lifecycle: Dream → Plan → Design → Implement → Test → Ship ([docs/vst-lifecycle.md](../../../docs/vst-lifecycle.md)).
2. DSP must obey [docs/dsp-architecture-specification.md](../../../docs/dsp-architecture-specification.md): no heap alloc, locks, or I/O in `processBlock`.
3. Prefer blocks from `vst/factory-lib/` and headers in `shared/dsp/` over one-off copies.
4. Unique `PLUGIN_MANUFACTURER_CODE` / `PLUGIN_CODE` in each plugin `CMakeLists.txt`.
5. Factory `factory-spec/v2` is the product contract; keep `dsp_modules` / `gui_modules` aligned with taxonomy.

## Build & QA

| Step | Command |
|------|---------|
| Compile | `tools/build-vst.sh <Name>` |
| Factory QA | `python3 vst-testing-ops/run_business.py --profile factory --plugin <Name>` |
| Certify | `POST /api/projects/{id}/certify` (hard gates in `testing/certification-v1.json`) |
| Ship | `tools/package-vst.sh <Name>` → `release/<Name>/` |

## Agent lanes

- **Factory director** routes work; does not edit `processBlock`.
- **Cursor Factory** implements JUCE/C++.
- **plugin-factory-liaison** writes PM prompts only.
