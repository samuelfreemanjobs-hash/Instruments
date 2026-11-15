# Overnight Agent

Synthesize original drum kits all night, mutate params, score WAVs, keep the best. Optional JUCE VST3 sampler loads `output/best_kit/` at runtime — **no rebuild every iteration**, and **never** copies into `Program Files\VST3`.

## Quick start (Windows)

```powershell
cd overnight-agent
pip install -r requirements.txt
python run_overnight.py --iterations 2 --hours 1
python run_overnight.py --hours 8
```

Morning report: `output/morning_report.md`  
Best kit: `output/best_kit/` + `output/best_kit.json`  
Log: `output/agent.log`

Optional LLM mutations: set env `MISTRAL_API_KEY` (never commit keys).

## Plugin (optional, once)

```powershell
cmake -S plugin -B plugin/build -G "Visual Studio 17 2022" -A x64 -DCOPY_PLUGIN_AFTER_BUILD=OFF
cmake --build plugin/build --config Release
python run_overnight.py --iterations 1 --build-plugin
```

Point the sampler at `output/best_kit` (see plugin README).

## Design choices

| Claimed cloud design | This repo |
|----------------------|-----------|
| Rebuild VST3 every iteration | Kits on disk; rebuild only with `--build-plugin` |
| `COPY_PLUGIN_AFTER_BUILD` on | Forced **OFF** |
| Samples required | Fully synthesized numpy kits |

Daytime DSP lab: `../vst/DrumDesignLab` + Intelligent Drum Lab.
