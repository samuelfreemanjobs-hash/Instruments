# Plugin format compatibility

Factory specs carry a `formats` object. **Implement** only what is `true` in CMake.

## Priority

| Format | MVP | Notes |
|--------|-----|--------|
| VST3 | Yes | Primary shipping target (Windows x64) |
| Standalone | Yes | JUCE `FORMATS VST3 Standalone` on APC template |
| AU | Later | macOS; requires Mac CI or local Xcode |
| CLAP | Later | See root `clap_juce_extensions` on JD Upgraded |
| AAX | Advanced | Avid SDK + legal agreement required |
| LV2 | Optional | Linux niche |

## Spec flags (factory-spec/v2)

```json
"formats": {
  "vst3": true,
  "standalone": true,
  "au": false,
  "clap": false,
  "aax": false,
  "lv2": false
}
```

## CMake (V2)

Use [vst/factory-lib/cmake/Formats.cmake](../../vst/factory-lib/cmake/Formats.cmake) to map spec → `juce_add_plugin(FORMATS ...)`.

MVP scaffolds from `vst/template` always emit VST3 + Standalone only.
