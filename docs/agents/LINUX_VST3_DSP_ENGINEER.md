# Linux VST3 DSP Engineer — agent knowledge index

**Fleet agent id:** `linux-vst3-dsp-engineer`  
**Entry:** [`disklordz/agents/profit/linux-vst3-dsp-engineer/agent.md`](../../disklordz/agents/profit/linux-vst3-dsp-engineer/agent.md)  
**Cursor skill:** [`.cursor/skills/linux-vst3-dsp-engineer/SKILL.md`](../../.cursor/skills/linux-vst3-dsp-engineer/SKILL.md)

This document is the **architecture + research spine** for Linux VST3 work in the Instruments monorepo (desktop x86_64 and embedded **aarch64**, including **Akai MPC Sample / Melodee**).

## Role boundaries

| Agent | Owns | Does not |
|-------|------|----------|
| **linux-vst3-dsp-engineer** | JUCE DSP, Linux VST3 build/cross-build, APC implement/test, MPC Sample targeting | SaaS, Stripe, factory HTTP foreman |
| **vst-factory-director** | Factory portfolio, gates, WO routing | `processBlock` edits |
| **plugin-factory-liaison** | CHORD queue, Cursor prompts | C++ implementation |
| **golden-wav-qa** | Golden WAV / regression policy | Feature DSP |

## Platform matrix (decision surface)

| Host | Binary | Toolchain in this repo |
|------|--------|-------------------------|
| Linux DAW x86_64 | `Contents/x86_64-linux/*.so` | `./tools/build-vst.sh <Name>` |
| Melodee on MPC Sample | `Contents/aarch64-linux/*.so` | `./tools/build-vst-aarch64-linux.sh <Name>` |
| MPC OS standalone | VST2 `.so` + MPC skin | [open-mpc/mpc-vst-plugins](https://github.com/open-mpc/mpc-vst-plugins) (not JUCE VST3) |
| Stock MPC Sample app | Limited third-party path | Open MPC `docs/MPC_SAMPLE.md` |

Melodee installs JUCE VST3 bundles under **`Melodee/Plugins/`** on the SD card. Verify with:

```bash
./scripts/vst/verify-mpc-sample-vst3.sh path/to/Plugin.vst3
```

See [`docs/MPC_SAMPLE_VST3.md`](../MPC_SAMPLE_VST3.md).

## Hardware facts (RK3566 / AC50) — [inspected]

Consolidated from [open-mpc/mpc-vst-plugins `docs/MPC_SAMPLE.md`](https://github.com/open-mpc/mpc-vst-plugins/blob/main/docs/MPC_SAMPLE.md), [keremimo/sample-base](https://github.com/keremimo/sample-base), and [inmusic az0x source](https://github.com/inmusic-software/az0x-open-source/tree/mpc-sample/1.3.0):

| Item | Value |
|------|--------|
| SoC | Rockchip RK3566, **4× Cortex-A55** @ 1.8 GHz |
| RAM | 2 GB |
| Audio | 44.1 kHz, **128-sample** periods typical |
| OS | Linux, **glibc 2.39**, aarch64, PREEMPT_RT |
| UI (stock / sample-base) | 320×240 SPI, **CPU drawing** (GPU disabled in DT) |
| Melodee host | VST3, LV2, CLAP, Open MPC folders, VST2 arm |

Design implication: treat Melodee like a **four-little-core** embedded Linux box; prefer **low polyphony**, **small GUIs**, and **`-march=armv8-a` / A55-friendly** code.

## Upstream GitHub (clone set)

| Repo | Why |
|------|-----|
| [keremimo/melodee-sample](https://github.com/keremimo/melodee-sample) | Plugin host rules, SD install, Docker arm64 build |
| [keremimo/sample-base](https://github.com/keremimo/sample-base) | ALSA/MIDI/display runtime for AC50 |
| [keremimo/mpc-vst-melodee-chip](https://github.com/keremimo/mpc-vst-melodee-chip) | Reference **light** synth; Open MPC port + Melodee zip |
| [open-mpc/mpc-vst-plugins](https://github.com/open-mpc/mpc-vst-plugins) | MPC OS ports, **MPC_SAMPLE.md**, optional `build_vst3.sh` |
| [inmusic-software/az0x-open-source](https://github.com/inmusic-software/az0x-open-source) (`mpc-sample/1.3.0`) | Kernel/DT reference |
| [inmusic-software/mpc-plugin-builder-sdk](https://github.com/inmusic-software/mpc-plugin-builder-sdk) | Akai-native plugin SDK |

**Licences:** Keremimo stack is **AGPL-3.0**; Open MPC toolchain **MIT**. Do not copy AGPL sources into closed plugins without compliance review.

## In-repo pipeline (APC + JUCE)

```
plugins/<Name>/  Dream → Plan → Design artifacts
        ↓
tools/new-vst.sh <Name>  →  vst/<Name>/
        ↓
Program chain (Source/Dsp, processBlock)  →  Surface (APVTS, Editor)
        ↓
tools/build-vst.sh | build-vst-aarch64-linux.sh
        ↓
vst-testing-ops/run_business.py  →  tools/package-vst.sh
```

Mandatory reads:

- [`docs/vst-lifecycle.md`](../vst-lifecycle.md)
- [`docs/dsp-architecture-specification.md`](../dsp-architecture-specification.md)
- [`docs/CLAUDE_VST_ENGINEER.md`](../CLAUDE_VST_ENGINEER.md)
- [`.agents/skills/audio-plugin-coder/SKILL.md`](../../.agents/skills/audio-plugin-coder/SKILL.md)

## Modular skills (agent)

Load from [`disklordz/agents/profit/linux-vst3-dsp-engineer/skills/`](../../disklordz/agents/profit/linux-vst3-dsp-engineer/skills/):

| Skill file | Topic |
|------------|--------|
| `01-session-bootstrap.md` | What to read first, memory recall |
| `02-dsp-realtime-architecture.md` | RT rules, chains, complexity |
| `03-juce-linux-x86_64.md` | Desktop build, pluginval, CI |
| `04-juce-linux-aarch64.md` | Cross-compile, juceaide, multiarch |
| `05-mpc-melodee-host.md` | Install layout, Open MPC vs JUCE |
| `06-apc-implement-test-ship.md` | Gates, commands, evidence |
| `07-open-mpc-porting.md` | When to use vst.json / mpc-vst-plugins |

## Success evidence checklist

- [ ] `cmake --build` succeeded for intended target (`<Name>_VST3` only on cross-builds)
- [ ] `file` reports **ELF 64-bit LSB … ARM aarch64** for Melodee bundles
- [ ] No new audio-thread allocations (grep review)
- [ ] `python3 vst-testing-ops/run_business.py --profile ci` or `dsp-only` when C++ DSP changed
- [ ] `python3 scripts/agent_memory.py check`