---
name: hermes-elite-qa
description: Plugin QA — pluginval, host smoke checklist, CI waivers. Use after Junova-X build changes.
---

# Hermes elite QA

1. Build: `cmake --build build -j --target JunovaX_VST3 JunovaX_CLAP JunovaX_Standalone`.
2. pluginval: `build/tools/pluginval/pluginval --validate-in-process --strictness-level 5 --file-or-id build/Junova-X/JunovaX_artefacts/Release/VST3/Junova-X.vst3`
3. Smoke: MIDI note, Diag test tone, Panic, HPF toggle, chorus modes, preset/state reload when implemented.
4. Headless CI: skip GUI pluginval tests per repo policy; document waivers in PR.
5. Attach SUCCESS log excerpt to PR body.
