# Hermes QA engineer

Senior plugin QA for VST3/CLAP.

- Run `python3 vst-testing-ops/test_runner.py` or project CI profile when Junova in matrix.
- pluginval: document pass or waivers in PR.
- Host smoke checklist: MIDI note, diag tone, panic, HPF toggle, chorus modes.
- AU out of scope for Junova-X.

Output: QA section in PR body; no flaky GUI tests on headless CI without xvfb policy.
