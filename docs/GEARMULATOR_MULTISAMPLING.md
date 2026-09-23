# Multisampling Gearmulator VSTi (Cloud VM vs sandbox)

## Short answer

| Approach | Multisample batch renders? | Notes |
|----------|----------------------------|--------|
| **This Cloud Agent VM** | **Yes** — headless, scripted | Use `Vst3OfflineRender` + `render_multisample_grid.sh`. No DAW GUI required. |
| **Docker / container (local or CI)** | **Yes** — isolated, reproducible | Image `instruments-multisample`; mount `/plugins`, `/rom`, `/out`. See below. |
| **Cursor Environment snapshot** | **Yes** — same VM, pre-baked deps | Extend `.cursor/environment.json` `install` to build `Vst3OfflineRender` and optionally Gearmulator plugins. |
| **Vercel / web “sandbox”** | **No** | For Next.js/serverless; not for VST3 + ROM workloads. |
| **Interactive plugin UI** | Limited | Cloud Agents can use computer-use for occasional tweaks; batch work should stay headless. |

Cloud Agent pods **do not include Docker**; use the VM scripts here, or run the container on your laptop / a CI runner with Docker installed.

Treat multisampling as an **offline batch job** in the VM: load VST3 → MIDI note grid → 24-bit WAV → `manifest.tsv` → **`instrument.map.json`** ([MULTISAMPLE_MAP_FORMAT.md](MULTISAMPLE_MAP_FORMAT.md)).

## Prerequisites

1. **Legal ROM** for the Gearmulator device (JP-8000, Virus, etc.) on disk — **never commit** to git.  
   Use a Cloud **secret** or path outside the repo, e.g. `GEARMULATOR_ROM_DIR` pointing at your dump folder.
2. **Built VST3** from your [fork](GEARMULATOR_FORK.md) or upstream submodule.
3. **`Vst3OfflineRender`** from root CMake (Instruments tree, not GPLv3 combined with JD sources in one binary — this host is a separate executable).

```bash
cmake --build build -j --target Vst3OfflineRender
```

## Single note (smoke test)

Works with any monorepo VST3 (e.g. Wave909) before Gearmulator is built:

```bash
./build/Vst3OfflineRender \
  --plugin "build/Wave909/Wave909_artefacts/Release/VST3/WAVE-909.vst3" \
  --out /tmp/note60.wav \
  --program 0 --note 60 --velocity 100 --seconds 3 --sampleRate 48000
```

## Full note grid (multisample session)

```bash
./scripts/gearmulator/render_multisample_grid.sh \
  --plugin "/path/to/JE-8086.vst3" \
  --name je8086_factory_001 \
  --program 0 \
  --note-start 36 --note-end 84 --note-step 3 \
  --velocity 100 --seconds 4
```

Output (gitignored):

```
gearmulator-lane/multisamples/out/je8086_factory_001_p0_v100/
  instrument.map.json
  manifest.tsv
  note_036_vel_100.wav
  ...
```

Tune `--note-step` (semitone = 1, minor third = 3) and add velocity passes by re-running with `--velocity` and a new `--name`.

## Gearmulator plugin build (VM)

After submodule init and ROM on disk (see upstream docs):

```bash
git submodule update --init --recursive gearmulator-lane/gearmulator
# build selected synth VST3 in gearmulator-lane/build/ … (upstream build_linux.sh)
```

Long builds are a good fit for a **dedicated Cursor Environment** so every agent does not recompile from cold.

## “Sandbox” in practice

**Recommended:** a **Cursor Environment** (repository `.cursor/environment.json` or dashboard snapshot) that:

- Installs Linux deps from `gearmulator-lane/gearmulator/linux_dependencies.sh`
- Runs `git submodule update --init --recursive gearmulator-lane/gearmulator`
- Builds `Vst3OfflineRender` with the main Instruments CMake preset
- Optionally caches a Release JE-8086 (or Osirus) VST3 in the snapshot **without** ROM (ROM still supplied per run via secret path)

## Docker container (local / CI)

Files: [gearmulator-lane/docker/Dockerfile](../gearmulator-lane/docker/Dockerfile), [docker-compose.yml](../gearmulator-lane/docker/docker-compose.yml).

**Build image** (from repo root):

```bash
./scripts/gearmulator/docker-multisample.sh build
```

**Run a grid** — copy or symlink your `.vst3` into `gearmulator-lane/docker/volumes/plugins/`, ROM into `volumes/rom/` (gitignored), then:

```bash
./scripts/gearmulator/docker-multisample.sh run \
  --plugin /plugins/JE-8086.vst3 \
  --name jp001 --program 0 \
  --note-start 36 --note-end 84 --note-step 3
```

Or with explicit host paths:

```bash
PLUGINS_DIR=$HOME/vst3 ROM_DIR=$HOME/jp8000-rom OUT_DIR=$PWD/out \
  ./scripts/gearmulator/docker-multisample.sh compose \
  --plugin /plugins/JE-8086.vst3 --name jp001
```

The image contains **`Vst3OfflineRender` only** — not Gearmulator sources. Build JE-8086 / Osirus VST3 on the host (or in a separate builder container on your fork) and mount the bundle under `/plugins`. **Never `COPY` ROM into the image**; always bind-mount `/rom` read-only.

## ROM and output policy

- WAV outputs under `gearmulator-lane/multisamples/out/` are **gitignored** by default.
- Redistribution of multisamples derived from firmware emulation is **your compliance responsibility**.
- Do not check captured WAVs or ROMs into Instruments or public fork branches unless rights are clear.

## Related

- [GEARMULATOR_LANE.md](GEARMULATOR_LANE.md) · [GEARMULATOR_FORK.md](GEARMULATOR_FORK.md)  
- [tools/ARCHITECTURE.md](../tools/ARCHITECTURE.md) — `Vst3OfflineRender`  
- JD clean-room multisample metadata: [ROM.md](ROM.md)
