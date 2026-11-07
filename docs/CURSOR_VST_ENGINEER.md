# Cursor as a VST generation powerhouse

This guide turns Cursor from a generic code assistant into a **JUCE-focused VST engineer** in this monorepo. It targets the two bottlenecks in AI plugin work: **token context management** and **automated compile/fix loops**.

Pair with [CURSOR_AGENT_PLAYBOOK.md](CURSOR_AGENT_PLAYBOOK.md), [AGENTS.md](../AGENTS.md), and [vst-testing-ops/ARCHITECTURE.md](../vst-testing-ops/ARCHITECTURE.md).

---

## 1. VST Engineer system prompt

Cursor loads project rules before Composer / Agent runs.

| Mechanism | Path | When it applies |
|-----------|------|------------------|
| **Project rules (preferred)** | [`.cursor/rules/vst-engineer.mdc`](../.cursor/rules/vst-engineer.mdc) | Files under `Source/`, `Wave9090/`, `SP1200/`, `MyFirstPlugin/` |
| **Legacy root rules** | [`.cursorrules`](../.cursorrules) | Same DSP contract for setups that still read `.cursorrules` |

Both enforce realtime audio safety (no alloc/logging in `processBlock`, APVTS thread safety, parameter ID consistency).

---

## 2. Framework docs via @Docs

Do not let the model guess JUCE APIs.

1. **Cursor → Settings → Features → Docs → Add New Doc**
2. URL: `https://docs.juce.com`
3. Name: `juce`
4. Optional second doc: `juce-modules` → `https://github.com/juce-framework/JUCE` (module READMEs)

**Composer example:** `@juce How do I register an AudioParameterChoice and attach it in AudioProcessorValueTreeState?`

For **DPF** experiments (not main CMake tree): add doc `dpf` → `https://github.com/DISTRHO/DPF`.

---

## 3. Autopilot compiling

### Cloud Agent / CI parity

```bash
python3 vst-testing-ops/run_business.py --profile ci       # full gate (matches build.yml)
python3 vst-testing-ops/run_business.py --profile dsp-only   # DSP / golden only
```

Failure handoff: read **`vst-testing-ops/error_log.txt`** (also surfaced in Streamlit `vst-testing-ops/app.py`).

### Fast local loop (build only)

```bash
./scripts/vst/autopilot-compile.sh
./scripts/vst/autopilot-compile.sh --target JDUpgraded_VST3   # single CMake target
```

Writes compiler errors to `vst-testing-ops/error_log.txt` when the build fails.

### IDE Terminal Agent (Ctrl+K / Cmd+K)

1. Open the integrated terminal.
2. **Ctrl+K** (Mac: **Cmd+K**).
3. Prompt: `Run ./scripts/vst/autopilot-compile.sh and fix any compiler errors until the build succeeds.`

The terminal agent reads the error log, patches sources, and re-runs — same contract as Cloud agents.

---

## 4. AI-friendly plugin map (context budget)

Large repos lose focus. **Always anchor feature work** on [VST_AI_PLUGIN_MAP.md](VST_AI_PLUGIN_MAP.md):

- Pin `#VST_AI_PLUGIN_MAP.md` or the product section in Composer.
- Keep **one product** in scope per task (JD Upgraded vs Wave9090 vs SP1200).

---

## 5. Token context hygiene

| Practice | Why |
|----------|-----|
| [`.cursorignore`](../.cursorignore) excludes `build/`, large ROMs | Stops indexing megabytes of artefacts |
| Read product `ARCHITECTURE.md` first | Structural truth in fewer tokens |
| Use `vst-testing-ops` profiles | One command instead of pasting build logs into chat |
| RAG (optional): `disklordz/rag/scripts/query_local.py` | Repo-specific prompts without full-tree @codebase |

---

## 6. Success checklist (agent)

- [ ] Edited only modules listed in VST_AI_PLUGIN_MAP for this product  
- [ ] `autopilot-compile.sh` or `run_business.py --profile ci` green  
- [ ] Golden WAVs updated only if DSP output intentionally changed (`tests/golden/refresh_golden.sh`)  
- [ ] `ARCHITECTURE.md` / map updated if file roles or line anchors moved materially  
