# JUCE factory — Disklordz agent guide

How this monorepo builds JUCE products, what to reuse, and what **not** to pull in from the wider ecosystem ([awesome-juce](https://github.com/sudara/awesome-juce) and similar lists).

**Read order for agents:** [ARCHITECTURE.md](../ARCHITECTURE.md) → this file → [docs/ARCHITECTURE.md](ARCHITECTURE.md) or [MyFirstPlugin/NEW_PLUGIN.md](../MyFirstPlugin/NEW_PLUGIN.md) depending on the task.

---

## Two tracks (do not mix without a work order)

| Track | When | Build | Docs |
|-------|------|-------|------|
| **JUCE factory** (default) | VST3/CLAP/standalone in this repo | Root `CMakeLists.txt`, JUCE **8.0.15** FetchContent | [INTEGRATION.md](INTEGRATION.md), [HANDOFF.md](HANDOFF.md) |
| **HISE sketch lane** | Rompler/sampler R&D only | HISE export on Windows; **not** root CMake | [HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md), [HISE_SKETCH_LANE.md](HISE_SKETCH_LANE.md) |

Promoting a HISE sketch to factory JUCE requires an explicit **port work order** — do not edit both lanes in one change unless instructed.

---

## What we already ship (use this first)

| Need | Location |
|------|----------|
| New effect or synth (CMake) | [MyFirstPlugin/](../MyFirstPlugin/) — `NEW_PLUGIN.md`, effect + `MyFirstSynth` |
| Composer / processor-first prompt | [MyFirstPlugin/CURSOR_COMPOSER_PROMPT.md](../MyFirstPlugin/CURSOR_COMPOSER_PROMPT.md) |
| Production synth reference | `JDUpgraded` in root CMake — [docs/ARCHITECTURE.md](ARCHITECTURE.md) |
| CLAP | [INTEGRATION.md](INTEGRATION.md) — pinned `clap-juce-extensions` commit |
| Headless regression | `OfflineRender`, `SpectralDiff`, [tests/golden/](../tests/golden/) |
| CI contract | [.github/workflows/build.yml](../.github/workflows/build.yml) |

**JUCE pins:** Factory root uses **8.0.15** (FetchContent). `MyFirstPlugin/` reads [JUCE_VERSION](../JUCE_VERSION) (may differ — align deliberately when bumping).

---

## Ecosystem survey — useful vs ignore

Curated after reviewing awesome-juce entries and related repos. **Discovery only** — do not submodule these into root CMake unless a work order says so.

### Use as reference or future CI (not templates)

| Resource | Use for Disklordz |
|----------|-------------------|
| [pamplejuce](https://github.com/sudara/pamplejuce) | **CI patterns:** Catch2, **pluginval**, GitHub Actions — gap vs our build-only CI |
| [pluginval](https://github.com/Tracktion/pluginval) | Post-build plugin validation (add job when ready) |
| [tomoyanonymous/juce_cmake_vscode_example](https://github.com/tomoyanonymous/juce_cmake_vscode_example) | **Local DevEx:** `.code-workspace`, clangd + CMake Tools, debug Standalone |
| [hollance/synth-plugin-book](https://github.com/hollance/synth-plugin-book) | Teaching / synth patterns (MIT) |
| [clap-juce-extensions](https://github.com/free-audio/clap-juce-extensions) | Already integrated — see INTEGRATION.md |

### Do not adopt as factory defaults

| Resource | Why skip |
|----------|----------|
| [TheAudioProgrammer/JuceAudioPluginTemplate](https://github.com/TheAudioProgrammer/JuceAudioPluginTemplate) | Minimal, JUCE on `master`, no APVTS — weaker than `MyFirstPlugin` |
| [TheAudioProgrammer/juceSynthFramework](https://github.com/TheAudioProgrammer/juceSynthFramework) | Projucer, JUCE 5 era, tutorial git history only |
| [tobanteAudio/juce-cookbook](https://github.com/tobanteAudio/juce-cookbook) | Stale GitBook outline; use topic list only |
| [react-juce](https://github.com/JoshMarler/react-juce) | Duktape React UI, inactive; wrong shape for JD editor |
| [benkuper/juce_organicui](https://github.com/benkuper/juce_organicui) | GPL app framework, **custom JUCE fork** — Chataigne-style apps only |
| [McMartin/FRUT](https://github.com/McMartin/FRUT) | Projucer → CMake migration; we have no `.jucer` projects |
| [create-juce-app](https://github.com/ImJimmi/create-juce-app) | Optional greenfield **outside** monorepo |

---

## Standard operating procedures

### SOP A — Change JD Upgraded or factory CMake

1. Read [ARCHITECTURE.md](../ARCHITECTURE.md) and [docs/ARCHITECTURE.md](ARCHITECTURE.md).
2. Configure and build from repo root (g++-12 on Linux CI):

   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release \
     -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
   cmake --build build -j
   ```

3. Run smoke path CI uses: build artefacts, `OfflineRender` ×2, [tests/golden/verify_golden.sh](../tests/golden/verify_golden.sh).
4. **Audio thread:** no alloc, no locks, no `DBG()` in `processBlock` — see CURSOR_COMPOSER_PROMPT.md.

### SOP B — New plugin SKU in monorepo

1. Follow [MyFirstPlugin/NEW_PLUGIN.md](../MyFirstPlugin/NEW_PLUGIN.md).
2. Paste [CURSOR_COMPOSER_PROMPT.md](../MyFirstPlugin/CURSOR_COMPOSER_PROMPT.md) into Composer with metadata filled in.
3. Register target in root or subproject CMake; add row to root [ARCHITECTURE.md](../ARCHITECTURE.md) if it is a new product.

### SOP C — Linux: JUCE examples / DemoRunner (optional dev)

JUCE FetchContent path: `build/_deps/juce-src` after a root configure.

Extra apt packages (GUI examples):

```bash
sudo apt-get install -y libgtk-3-dev libwebkit2gtk-4.1-dev
```

```bash
cd build/_deps/juce-src
cmake . -B cmake-build -DJUCE_BUILD_EXAMPLES=ON -DJUCE_BUILD_EXTRAS=ON
cmake --build cmake-build --target DemoRunner -j
```

Not required for JD Upgraded CI.

### SOP D — Ingest external Projucer project (rare)

1. Spike with [FRUT](https://github.com/McMartin/FRUT) `Jucer2CMake` (GPL — compliance is your call).
2. Target end state: native `juce_add_plugin` like `MyFirstPlugin`, not long-term Reprojucer.
3. Do not merge until CLAP/pin policy matches [INTEGRATION.md](INTEGRATION.md).

### SOP E — Pick a third-party JUCE module from awesome-juce

1. License must fit the product (prefer MIT/BSD for commercial SKUs).
2. Prefer **pinned tag** + FetchContent; no JUCE fork unless work order allows it.
3. Document in product `ARCHITECTURE.md`.

---

## Prompts and workflows (already in repo)

| Artifact | Role |
|----------|------|
| `CURSOR_COMPOSER_PROMPT.md` | Processor-first, RT-safe rules |
| `NEW_PLUGIN.md` | Rename targets, APVTS checklist |
| `docs/HANDOFF.md` | Product state for agents |
| `.cursor/environment.json` | Cloud VM: Linux deps + default configure/build |

**Not needed:** Duplicate boilerplate from pamplejuce/TAP/tomoy repos — only **cherry-pick** CI (pluginval) or VS Code workspace when someone asks.

---

## Recommended follow-ups (priority)

1. **pluginval** in CI on built VST3/CLAP (model on pamplejuce) — quality gate, not a template.
2. **Optional** `disklordz.code-workspace` at root for local clangd (from tomoy example) — DevEx only.
3. **Optional** weekly script to parse awesome-juce `sites.txt` → JSON — only if discovery noise becomes a problem; manual list above is enough for now.

---

## Related

- [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md) — product tracks A–C
- [UI.md](UI.md) — JD Upgraded editor layout
- [AB_HARNESS.md](AB_HARNESS.md) — offline render harness
