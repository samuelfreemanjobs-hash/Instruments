# mpc-agent + DawDreamer (multisample render lane)

**mpc-agent repo:** https://github.com/samuelfreemanjobs-hash/mpc-agent  
**DawDreamer:** https://github.com/DBraun/DawDreamer · docs https://dirt.design/DawDreamer/

Akai **MPC Software** multisampler workflows and **Gearmulator VSTi** capture share the same need: **offline, automated VST renders** (note grids, velocities, presets) without a manual DAW session.

This monorepo hosts the **proven Linux render scripts**; **mpc-agent** is the product/agent home for MPC-specific packaging (programs, keys, export to MPC Software / SFZ).

---

## Why DawDreamer (vs C++ `Vst3OfflineRender` only)

| | **DawDreamer (Python)** | **Vst3OfflineRender (JUCE C++)** |
|--|-------------------------|----------------------------------|
| Host | Python `RenderEngine` | Native binary in `build/` |
| Automation | MIDI, PPQN, parameter automation, graphs | Single-plugin note render |
| mpc-agent fit | Primary — agent logic in Python | Fallback / no Python on image |
| Linux VST3 | Yes (tested with WAVE-909.vst3 in Cloud VM) | Yes |
| Presets | `.fxp` (VST2), `.vstpreset` (VST3) | Program index only |

Use **both**: DawDreamer for rich mpc-agent automation; JUCE host for minimal Docker images without Python wheels.

---

## Setup (Cloud VM or local)

```bash
pip install -r gearmulator-lane/requirements-dawdreamer.txt
# or
./scripts/gearmulator/install-dawdreamer.sh
```

Build a plugin to render (factory or Gearmulator on your fork):

```bash
cmake --build build -j --target Wave909_VST3   # example
```

---

## Render a multisample grid

```bash
python3 scripts/gearmulator/render_multisample_dawdreamer.py \
  --plugin build/Wave909/Wave909_artefacts/Release/VST3/WAVE-909.vst3 \
  --name wave909_lead \
  --program 0 \
  --note-start 36 --note-end 84 --note-step 3 \
  --velocity 100 --seconds 3
```

Output: `gearmulator-lane/multisamples/out/<session>/` + **`instrument.map.json`** (canonical) + `manifest.tsv` (column `engine=dawdreamer`).

Gearmulator JE-8086 / Osirus: build VST3 from [gearmulator-lane](gearmulator-lane/) submodule, mount ROM per upstream docs, pass `--plugin /path/to/Plugin.vst3`.

---

## mpc-agent submodule

```bash
git submodule update --init --recursive mpc-agent
```

Future **MPC Software** exporters, preset naming, and agent prompts live in **mpc-agent**; read **`instrument.map.json` v1** ([MULTISAMPLE_MAP_FORMAT.md](MULTISAMPLE_MAP_FORMAT.md)) or call `finalize_multisample_session.py` / render scripts via subprocess.

Cloud Agents need token access to mpc-agent — `.cursor/environment.json` lists `repositoryDependencies` for that repo.

---

## Autonomous agent loop (mpc-agent + Instruments)

```text
1. mpc-agent WO: target MPC kit + source VST (Gearmulator or factory)
2. Render: render_multisample_dawdreamer.py (or Docker multisample container)
3. Post: trim/normalize → SFZ / MPC program (mpc-agent)
4. Optional: HISE rompler path — docs/GEARMULATOR_TO_SYNTH_PATHS.md
```

**JUCE factory elite worker** ([JUCE_FACTORY_AGENT.md](JUCE_FACTORY_AGENT.md)) builds plugins; **mpc-agent** consumes them for sample libraries.

End-to-end **multisample → new instrument**: [MULTISAMPLE_TO_INSTRUMENT_WORKFLOW.md](MULTISAMPLE_TO_INSTRUMENT_WORKFLOW.md).

---

## Policy

- No ROM, firmware dumps, or retail WAVs in git.
- GPLv3 Gearmulator stays separate from JD Upgraded `Source/`.
- Plugin compatibility: some VST3 presets need `.vstpreset`; VST2 builds may load `.fxp` more reliably (see DawDreamer issues).

---

## Related

- [GEARMULATOR_MULTISAMPLING.md](GEARMULATOR_MULTISAMPLING.md) (when Track E merged) — VM + Docker  
- [GEARMULATOR_TO_SYNTH_PATHS.md](GEARMULATOR_TO_SYNTH_PATHS.md) — rompler from captures  
- [mpc-agent/README.md](../mpc-agent/README.md) (submodule)
