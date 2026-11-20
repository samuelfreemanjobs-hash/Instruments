# VST Engineering — reference

## Linux cloud / CI (default in AGENTS.md)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
cmake --build build -j --target <CMakeTarget>_VST3
```

Cap parallel jobs on memory-limited runners (e.g. `-j4` for large JUCE targets).

## Common CMake targets

| Product | Typical VST3 target | Artifact hint |
|---------|---------------------|---------------|
| APC plugin `vst/<Name>/` | `<Name>_VST3` | `vst/<Name>/build/<Name>_artefacts/Release/VST3/` |
| Prophet Rev2 Trap | `ProphetRev2Trap_VST3` | `build/ProphetRev2Trap/ProphetRev2Trap_artefacts/Release/VST3/` |
| SP-1200 | `SP1200_VST3`, `SP1200_Standalone` | `build/` + `ctest -R SP1200` |

Always read the product `ARCHITECTURE.md` for the authoritative target name.

## Handoff docs (multi-agent)

| Doc | Product |
|-----|---------|
| `docs/SP1200_HANDOFF.md` | SP-1200 Drumulator |
| `docs/PROPHET_REV2_TRAP_HANDOFF.md` | Prophet Rev2 Trap |

## APC folders

| Path | Contents |
|------|----------|
| `plugins/<Name>/` | `creative-brief.md`, `parameter-spec.md`, `architecture.md`, `design-spec.md`, `status.json` |
| `vst/<Name>/` | JUCE `Source/`, `CMakeLists.txt`, `params.json` |
| `release/<Name>/` | Shipped zip + `INSTALL.txt` |

## Scaffold & package

```bash
./tools/new-vst.sh <Name> --type synth
./tools/new-vst.sh <Name> --type effect
./tools/build-vst.sh <Name>
./tools/package-vst.sh <Name>
```

## QA profiles (`vst-testing-ops`)

| Profile | When |
|---------|------|
| `ci` | Before push; matches `.github/workflows` |
| `dsp-only` | DSP/golden iteration |
| `factory` | Scaffolded APC plugin: `--plugin <Name>` |

## Windows worker

On user Windows machines, use `.cursor/rules/windows-worker.mdc` — not the Linux `g++-12` commands above.

## Related docs

- `docs/CLAUDE_VST_ENGINEER.md` — full RT + SmoothParam + pluginval
- `docs/apc-workflow.md` — slash commands
- `vst/README.md` — template layout
