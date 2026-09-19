# DPCS — Build Methodology

**Cursor rule:** [`.cursor/rules/dpcp.mdc`](../.cursor/rules/dpcp.mdc) (`alwaysApply: true` on every agent task in this repo).

Agents and humans refer to this loop as **DPCP** — the default way to build **anything** here (JUCE plugins, `disklordz/website`, tools, docs-only work). It comes from the **Autonomous Builder / PLUGIN FACTORY OS** book prompt in [PLUGIN_FACTORY_OS_PROMPT.md](PLUGIN_FACTORY_OS_PROMPT.md).

| Letter | Phase | You must |
|--------|--------|----------|
| **D** | **Discover** | Read `/ARCHITECTURE.md`, `AGENTS.md`, product `ARCHITECTURE.md`, and `.cursor/rules/*`. Search the repo for reuse. **No bulk product code** until context is written down. |
| **P** | **Plan** (Specify) | Freeze intent in-repo: `PRODUCT_SPEC`, architecture, parameter/manifest skeletons, success criteria. Propose paths and CMake/npm targets before large diffs. |
| **C** | **Code** | **Vertical slices** only — one shippable step per commit. Each slice ends green: build, test, or documented N/A for docs-only. |
| **S** | **Ship** (Prove) | Evidence before PR: test output, `ctest`/`npm test`, pluginval when C++ DSP changed, walkthrough artifacts for UI. Draft PR; do not merge unless asked. |

**DPCP** uses the same four phases; **S** = **Prove** with automated checks + honest `NOT VERIFIED` for manual steps.

---

## Non-negotiables (every project)

1. **Product boundary** — Edit only the product folder named in the task. Do not conflate separate product lines (e.g. DISKLORDZ ROMPLER ≠ WAVE-909).
2. **Architecture rule** — Update `ARCHITECTURE.md` when you add targets, modules, or change data flow ([architecture-documentation.mdc](../.cursor/rules/architecture-documentation.mdc)).
3. **Security baseline** — [security-baseline.mdc](../.cursor/rules/security-baseline.mdc): no secrets in git; validate public inputs; authZ on user data.
4. **Branch policy** — `cursor/<description>-9a2b` (or team suffix); commit often; no force-push.
5. **Anti-patterns (hard stop)** — One-shot mega-diffs; claiming tests ran when they did not; secrets or copyrighted samples in git; skipping specify before implementation.

---

## Stack-specific Prove commands

| Area | Typical Prove step |
|------|---------------------|
| JUCE / plugin | `cmake --build build -j`, `ctest -R <Product>`, `python3 vst-testing-ops/run_business.py --profile ci` after DSP edits |
| Disklordz web | `cd disklordz/website && npm ci && npm run build && npm test` |
| Docs / rules only | Link check + no broken paths; no compile required |

See [AGENTS.md](../AGENTS.md) for the canonical matrix.

---

## Plugin factory (when building VSTi)

Use the full factory prompt in [PLUGIN_FACTORY_OS_PROMPT.md](PLUGIN_FACTORY_OS_PROMPT.md) for *what* to build; **DPCP** is always *how*. §0 in that file is the detailed plugin slice template (Phases A–D = DPCS).

Product-specific Cursor prompts (e.g. [DISKLORDZ_ROMPLER_CURSOR_PROMPT.md](DISKLORDZ_ROMPLER_CURSOR_PROMPT.md)) add scope on top of DPCP — they do not replace it.

---

## DiskLordz content platform (extra roadmap)

ROM Factory / 1,000-preset content work **still follows DPCP** for every slice. Additionally use [DisklordzSynth/Docs/CONTENT_PLATFORM_ROADMAP.md](../DisklordzSynth/Docs/CONTENT_PLATFORM_ROADMAP.md) for phased content delivery (optional Phase 0 audit: [CONTENT_PLATFORM_PHASE0_AUDIT.md](../DisklordzSynth/Docs/CONTENT_PLATFORM_PHASE0_AUDIT.md)).

---

## How to invoke

```text
Follow DPCP.
Follow DPCP from Discover only (docs commit).
Follow DPCP — vertical slice: <one sentence>.
```

Product-specific roadmaps (content Phase N, ROMPLER UI tab, SaaS feature) must still exit each slice with **Prove**.
