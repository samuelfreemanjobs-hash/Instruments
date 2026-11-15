# Overnight Agent — DiskLordz Drum Kit Evolution

## Purpose

Unattended overnight loop that synthesizes original drum kits, mutates parameters, scores results, and keeps the best kit. Optional JUCE VST3 sampler loads kits from disk at runtime (no rebuild per iteration).

## Build & run

```powershell
cd overnight-agent
pip install -r requirements.txt
python run_overnight.py --hours 8
# smoke:
python run_overnight.py --hours 0.01 --iterations 1
```

Optional plugin (once, not every iteration):

```powershell
cmake -S plugin -B plugin/build -G "Visual Studio 17 2022" -A x64 -DCOPY_PLUGIN_AFTER_BUILD=OFF
cmake --build plugin/build --config Release
```

## Data flow

```text
params JSON → numpy synth → WAVs under output/kits/<iter>/
        → score (format + energy + diversity)
        → mutate (random walk or optional Mistral)
        → state.json / best_kit.json / agent.log
        → morning_report.md
```

VST3 (optional) reads `KIT_DIR` / last `best_kit.json` path — **never** copies into `Program Files\VST3`.

## Real-time / safety

- Python synth is offline only.
- Plugin: no alloc in audio callback beyond voice state; kit load on message thread.
- `COPY_PLUGIN_AFTER_BUILD` forced OFF.
- No API keys in git; `MISTRAL_API_KEY` from env only.

## Related

- `../vst/DrumDesignLab` — primary DSP lab; this agent is a lightweight overnight harness.
- `../vst/DrumDesignLab/docs/INTELLIGENT_DRUM_LAB.md` — prompt→DSP path for daytime work.
