# VST Factory — development team roster

Factory specialists are **internal** role prompts under `disklordz/vst-factory/agents/`. One **company foreman** (`vst-factory-director`) routes work and reads approval gates.

## Foreman (profit fleet)

| ID | Role |
|----|------|
| [vst-factory-director](../disklordz/agents/profit/vst-factory-director/agent.md) | Hybrid factory foreman: portfolio, gates, product WO; delegates C++ to Cursor |

## Internal factory roles

| Role file | Responsibility |
|-----------|----------------|
| [architect.md](../disklordz/vst-factory/agents/architect.md) | `factory-spec/v2`, taxonomy, acceptance |
| [dsp.md](../disklordz/vst-factory/agents/dsp.md) | factory-lib composition, DSP contracts |
| [dsp-research.md](../disklordz/vst-factory/agents/dsp-research.md) | Algorithm compare → `approve_dsp_promotion` |
| [cpp.md](../disklordz/vst-factory/agents/cpp.md) | APC/JUCE implementation slices |
| [gui.md](../disklordz/vst-factory/agents/gui.md) | GUI DSL, APVTS bindings |
| [midi.md](../disklordz/vst-factory/agents/midi.md) | MIDI/MPE modules |
| [sampler-architect.md](../disklordz/vst-factory/agents/sampler-architect.md) | Keygroup / slice maps |
| [preset-intelligence.md](../disklordz/vst-factory/agents/preset-intelligence.md) | Preset batches + previews |
| [build.md](../disklordz/vst-factory/agents/build.md) | build-vst, artefacts |
| [qa.md](../disklordz/vst-factory/agents/qa.md) | factory / factory-audio / certify |
| [repair.md](../disklordz/vst-factory/agents/repair.md) | CI/log triage → minimal fix plan |

## Adjacent company agents (not factory bench)

| ID | Lane |
|----|------|
| plugin-factory-liaison | CHORD / legacy queue, `PASTE_TO_FACTORY` |
| audio-business-copilot | Sprint and commercial roadmap |
| autonomous-coding-agent | Allowlisted small slices |

## Methodology

All greenfield C++ slices follow **[DPCS.md](DPCS.md)** (Constitution → PRD → Contract → one module/run → verify exit 0) inside APC Implement/Test.

## Gates (collaborative autonomy)

`approve_architecture` · `approve_dsp_promotion` · `approve_gui` · `approve_release` — see [OPERATING_MODES.md](../disklordz/vst-factory/standards/OPERATING_MODES.md).

## Entrypoints

```bash
cd disklordz/vst-factory && uvicorn vst_factory.main:app --host 127.0.0.1 --port 8790
python3 vst-testing-ops/run_business.py --profile factory --plugin <Name>
```

Index: [DISKLORDZ_VST_FACTORY_PLATFORM.md](DISKLORDZ_VST_FACTORY_PLATFORM.md)
