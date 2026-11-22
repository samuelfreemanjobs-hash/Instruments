---
name: overnight-kit-agent
description: >-
  Runs and extends the overnight-agent drum kit evolution loop (numpy synth,
  score, mutate, morning report, optional JUCE sampler). Use when the user
  mentions overnight agent, overnight loop, best_kit.json, or unattended kit
  mutation.
---

# Overnight Kit Agent

## Authority

- `overnight-agent/ARCHITECTURE.md`
- `overnight-agent/run_overnight.py`
- Windows: `COPY_PLUGIN_AFTER_BUILD` always **OFF**

## Design (do not regress)

| Wrong | Right |
|-------|-------|
| Rebuild VST3 every iteration | Kits on disk; plugin loads `output/best_kit/` |
| Copy into Program Files VST3 | Forced OFF |
| Sample libraries | Procedural numpy synth |

## Run

```powershell
cd overnight-agent
pip install -r requirements.txt
python run_overnight.py --iterations 2 --hours 1   # smoke
python run_overnight.py --hours 8                  # overnight
```

Outputs: `output/agent.log`, `state.json`, `best_kit.json`, `best_kit/`, `morning_report.md`

Optional LLM mutations: env `MISTRAL_API_KEY` only (never commit).

## Plugin (once)

```powershell
cmake -S plugin -B plugin/build -G "Visual Studio 17 2022" -A x64 -DCOPY_PLUGIN_AFTER_BUILD=OFF
cmake --build plugin/build --config Release
```

## Extending

- Prefer morning-report / scoring improvements over resampling modes unless asked
- Keep Device Guard in mind: new unsigned exes may need a copy rename on this laptop
