# Boom on Windows

Studio setup for **style → 100 VST3 one-shots** on Akai MPC workflows.

## Prerequisites

1. **Visual Studio 2022** (or Build Tools) with *Desktop development with C++*
2. **CMake** 3.22+ (`winget install Kitware.CMake`)
3. **Python 3.10+** — install from [python.org](https://www.python.org/downloads/) and enable **Add python.exe to PATH**
4. **Git** clone of this repo

Optional: `pip install pytest` for unit tests.

## One-time build

From **PowerShell** in the repo root:

```powershell
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target VstiOneShotRender -j
```

The renderer is usually at:

`build\Release\VstiOneShotRender.exe`

## Run Boom (recommended)

```powershell
.\scripts\boom.ps1 `
  -Style "dark Memphis melodic stabs and warm keys" `
  -Vst3 "C:\Program Files\Common Files\VST3\Serum.vst3" `
  -Out "$env:USERPROFILE\Exports\mpc_melody_pack"
```

The script builds `VstiOneShotRender` if it is missing.

### Flags

| Parameter | Default | Notes |
|-----------|---------|--------|
| `-Count` | 100 | Number of one-shots |
| `-MatchMode` | `hybrid` | `names` = faster |
| `-DryRun` | off | Recipe + job only, no WAVs |

## Manual Python entry

```powershell
py -3 tools\vsti-one-shot\boom.py `
  --style "YOUR STYLE" `
  --vst3 "C:\Program Files\Common Files\VST3\YourSynth.vst3" `
  --out "$env:USERPROFILE\Exports\pack"
```

Re-render a saved job (identical WAVs):

```powershell
py -3 tools\vsti-one-shot\boom.py --job "$env:USERPROFILE\Exports\pack\.boom_20261105T120000Z\job.json"
```

## Finding your VST3

Most plugins install as a **folder** ending in `.vst3`:

| Location | Examples |
|----------|----------|
| `C:\Program Files\Common Files\VST3\` | Serum, many modern synths |
| `C:\Program Files\Native Instruments\VSTPlugins 64 bit\` | Some NI products |

In File Explorer, copy the **full path to the `.vst3` folder** (not a DLL inside it).

## Output

```text
%USERPROFILE%\Exports\mpc_melody_pack\
  your_style_slug_001.wav
  ...
  your_style_slug_100.wav
  manifest.json
  .boom_<utc>\recipe.json, assignments.json, job.json
```

Copy WAVs into **MPC Sample** (or your sampler); map pads yourself — no keygroups are exported.

## Troubleshooting

| Issue | Fix |
|-------|-----|
| `VstiOneShotRender not found` | Run the one-time build above; check `build\Release\`. |
| Plugin fails to load | Run PowerShell **as Administrator** once, or install the plugin to a user-writable path; some DRM plugins need the vendor standalone first. |
| Hybrid run is slow | Use `-MatchMode names` for a faster first pass. |
| Python not found | Use `py -3` or reinstall Python with PATH enabled. |

## Related

- [VSTI_ONE_SHOT_HANDOFF.md](VSTI_ONE_SHOT_HANDOFF.md)
- [tools/vsti-one-shot/docs/SAMPLE_RECIPE_BOOK.md](../tools/vsti-one-shot/docs/SAMPLE_RECIPE_BOOK.md)
