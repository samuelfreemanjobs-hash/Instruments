# Vendored theme fonts (OFL)

All TTFs are licensed under the SIL Open Font License 1.1. Each family directory ships its `OFL.txt`. Plugins embed these via `juce_add_binary_data` (see `factory_gui_use_theme`).

| Family | Used by themes | Source | Version | Files |
|--------|----------------|--------|---------|-------|
| Inter | `disklordz`, `_template` | https://github.com/rsms/inter/releases/download/v4.1/Inter-4.1.zip | 4.1 | Regular, Medium, SemiBold, Bold |
| JetBrains Mono | `disklordz`, `_template` | https://github.com/JetBrains/JetBrainsMono/releases/download/v2.304/JetBrainsMono-2.304.zip | 2.304 | Regular, Medium, SemiBold, Bold |
| IBM Plex Sans | `studio-light` | https://github.com/IBM/plex (`packages/plex-sans/fonts/complete/ttf`) | master @ fetch (OFL) | Regular, Medium, SemiBold, Bold |
| IBM Plex Mono | `studio-light` | https://github.com/IBM/plex (`packages/plex-mono/fonts/complete/ttf`) | master @ fetch (OFL) | Regular, Medium, SemiBold, Bold |

## SHA-256 (TTF files)

```
288316099b1e0a47a4716d159098005eef7c0066921f34e3200393dbdb01947f  Inter/Inter-Bold.ttf
97ad806f526e41546d46365bb3a393145f75b7b1568913db74549ad8b8dba872  Inter/Inter-Medium.ttf
40d692fce188e4471e2b3cba937be967878f631ad3ebbbdcd587687c7ebe0c82  Inter/Inter-Regular.ttf
78a843fade9d4612a5567302fb595b56976eb5fcebf4fea5a5912d638bafcde3  Inter/Inter-SemiBold.ttf
5590990c82e097397517f275f430af4546e1c45cff408bde4255dad142479dcb  JetBrainsMono/JetBrainsMono-Bold.ttf
31c92d01a8a08528b718a43addf0ad3df0af2ca4b7b3290a452f70f358e14d3d  JetBrainsMono/JetBrainsMono-Medium.ttf
a0bf60ef0f83c5ed4d7a75d45838548b1f6873372dfac88f71804491898d138f  JetBrainsMono/JetBrainsMono-Regular.ttf
1b3bfa1ed5665a4ce3f9feb68d2d4e40e70bf8b4b7d9a3edd418f321b4e166a0  JetBrainsMono/JetBrainsMono-SemiBold.ttf
9e6c74a889a700d707613d24548fe4ffa6bc59559a0689d2cf9e133bdcdafb2f  IBMPlexSans/IBMPlexSans-Bold.ttf
331c8639d7598b2cde62a911a71db195e30cb655cd6bdf2e324a7e984955f907  IBMPlexSans/IBMPlexSans-Medium.ttf
975dcda37d80f038dcd143c22e33ca2d97a0cc5a929aace1c749153b0fe1afa5  IBMPlexSans/IBMPlexSans-Regular.ttf
a20caf8286023a6a7a85e40b1d2a4ae9fc3e3b1f9eda8f4c542dd4986af67bb1  IBMPlexSans/IBMPlexSans-SemiBold.ttf
74e5eedcfa4596497d34e19023cabdabd3a8c852b903007a5654a59591a72ffb  IBMPlexMono/IBMPlexMono-Bold.ttf
98fbd727aae340b236955879dabed4d991aac9e8e90b3b2a67ce4a59221cc97c  IBMPlexMono/IBMPlexMono-Medium.ttf
7c6fbddca4b700be918f5f6183d9bd4464fa427fe435f0b480d77fe2bb8c5a43  IBMPlexMono/IBMPlexMono-Regular.ttf
f04d7c488ddf7d1fa99f2574efc3406ea4cbe17bb1af3a1ab960f84d0c96a172  IBMPlexMono/IBMPlexMono-SemiBold.ttf
```

## Fallback

If a product builds with `-DFACTORY_GUI_USE_SYSTEM_FONTS=ON`, `FactoryLookAndFeel` skips binary-data typefaces and uses JUCE default sans/mono. Prefer the vendored OFL fonts for shipping plugins.
