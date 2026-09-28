# HISE + Antigravity lane (quick reference)

**Implementer:** Google Antigravity on **your Windows PC** (HISE + MSVC).  
**Not in scope:** Cursor Cloud Agent, root CMake CI, CLAP — unless you open a **JUCE port WO**.

Full policy: [HISE_SKETCH_LANE.md](HISE_SKETCH_LANE.md) · Airtable seed: [`hise-sketch-lane-2026.json`](../disklordz/airtable/seed/hise-sketch-lane-2026.json)

---

## Boundaries

| In lane | Out of lane |
|---------|-------------|
| Romplers, sample maps, HLAC, scripted sampler UI | Junova-X, NovaDrum circuit DSP, JD Upgraded |
| HiseScript + `XmlPresetBackups/*.xml` | Hand-editing exported C++ in `Binaries/` |
| Local **VST3** via HISE export | **CLAP**, Linux CI, pluginval in monorepo (JUCE factory) |
| SKUs after **Marketing brief + Planner `product_id`** | Selling under `DL-LANE-HISE-SKETCH` (process row only) |
| WO prefix **`[Plugin][HISE]`**, owner **`antigravity-hise`** | `[Plugin][Junova-X]` / Cursor WIP unless port WO |

**Promotion path when a sketch is worth selling:** Marketing brief → Planner creates **product row** → Antigravity keeps building **or** open **Cursor JUCE port WO** if the SKU must ship with **CLAP + monorepo CI**.

---

## CLI checklist (Windows)

Prereqs: VS 2022 C++ workload, HISE built from source, VST3 SDK in HISE tree, project saved once in HISE.

```batch
set HISE_EXE=C:\HISE-develop\projects\standalone\Builds\VisualStudio2022\x64\Release\App\HISE.exe
set HISE_SRC=C:\HISE-develop
set PROJECT=C:\HISE_Projects\YourProduct

"%HISE_EXE%" -h:%HISE_SRC%
"%HISE_EXE%" set_project_folder -p:"%PROJECT%"
"%HISE_EXE%" export_ci "XmlPresetBackups\YourProduct.xml" -t:instrument -p:VST3 -a:x64
call "%PROJECT%\Binaries\batchCompile.bat"
```

- Use **relative** XML path under `PROJECT` for `export_ci` (CI mode).
- **`export_ci` alone is not enough** — run `batchCompile.bat` and check both exit codes.
- Optional before full export: `compile_script`, `clean` (see [HISE CLI](https://docs.hise.dev/glossary/command-line-tool.html)).

Artifact: typically `PROJECT\Binaries\Compiled\` (VST3). Smoke in Reaper before calling a sketch “done.”

---

## When to open a JUCE port WO (Cursor factory)

Open a **`[Plugin][Junova-X]`-style port WO** (new `product_id`, e.g. `[Plugin][HISE-Port]`) when **any** of:

- SKU must ship **CLAP** or match **Instruments** `build.yml` / pluginval gates
- You need **Linux** build or long-term maintenance in `Junova-X/` / `Source/` patterns
- HISE export limits block the product (custom DSP, NovaDrum-class circuits, tight latency)
- GTM promises **demo + updates** tied to GitHub releases in this monorepo

**Stay on Antigravity + HISE** when:

- Windows **VST3-only** is acceptable for v1
- Product is sample/rompler-first and ships on your timeline without CLAP
- Sketch is still pre-brief (no Planner product row yet)

Port WO acceptance should name: behavior parity checklist, preset/sample migration, VST3+CLAP targets, CI proof — not “re-run export_ci.”

---

## How to give this to Antigravity

1. **Clone or pull** `Instruments` (branch `main` after PR #21).
2. **Pin this file** in the Antigravity project:
   - Paste path or contents into **project instructions** / **rules**, or
   - Add to the workspace Antigravity opens (same folder as your HISE projects).
3. **System instructions** (short):

   > You are the Disklordz HISE sketch implementer. Follow `docs/HISE_ANTIGRAVITY_LANE.md` and `docs/HISE_SKETCH_LANE.md`. Only work `[Plugin][HISE]` work orders. Do not modify JUCE trees unless a JUCE port WO exists. Use `write_hisescript` + compile tools with `HISE_EXE`, `HISE_SRC`, `PROJECT_DIR` from environment — never commit secrets.

4. **Tools:** wire `write_hisescript`, `compile_hise_vst` (export_ci + batchCompile), optional `compile_script` / `clean` as in [HISE_SKETCH_LANE.md](HISE_SKETCH_LANE.md).
5. **Work order:** after Airtable seed, run or assign **`WO-2026-HISE-001`** for lane bootstrap; for real SKUs, Planner links WO to the new **`product_id`** and Marketing brief path (`hise-sketch/<Name>/docs/brief.md` when in git).
6. **Handoff to Cursor:** comment on the Airtable WO + GitHub issue with `.vst3` path, Reaper notes, and “request JUCE port” flag — Planner opens the port WO.

Antigravity does **not** need Zapier or Cursor MCP for HISE; it needs local HISE, your WO text, and these docs.
