# VST Factory — target formats

## Primary

| Format | Status | Notes |
|--------|--------|-------|
| **VST3** | Required | Default `target_format` in factory-spec/v2; ship zip from `tools/package-vst.sh` |
| Standalone | Optional | Enabled per-plugin CMake when useful for dogfood |
| CLAP | Deferred | Not a factory MVP gate |
| AAX | V3 roadmap | Requires Avid license; out of Phase 1 |

## Host OS matrix (ship)

| OS | Factory build broker | Operator DAW smoke |
|----|----------------------|--------------------|
| Linux x64 | `linux_compile_only` via `tools/build-vst.sh` | Optional |
| Windows x64 | Local worker / GitHub Actions artifact | Required before commercial release |
| macOS | Deferred | Manual when tooling available |

## Packaging layout

```text
release/<Name>/
  <Name>-<version>-vst3.zip
  staging/<Name>.vst3/
  staging/INSTALL.txt
  staging/manifest.json
```

## Spec fields

`factory-spec/v2`: `target_format` (default `vst3`), `target_os` (default `windows_x64` for commercial SKUs; Linux CI still builds).
