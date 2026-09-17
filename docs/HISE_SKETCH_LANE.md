# HISE sketch lane (Antigravity)

**Quick ref (Antigravity handoff):** [HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md)

**Status:** Parallel R&D lane — not the ship stack for Junova-X or the TR-808 circuit plugin unless promoted in Airtable.  
**Implementer:** **Google Antigravity** (local Windows + HISE + MSVC), not Cursor Cloud Agent.  
**PM:** Airtable Disklordz OS · **Strategy:** Business Planner + Marketing own SKU fit and GTM before any “real” product row.

## Purpose

Use **HISE** (Hart Instruments Sampler Engine) to rapidly build **sample-based instruments and romplers** (and simple scripted synths) via **HiseScript** + automated **VST3** export. Outputs are **optional SKUs** that must align with product families, price ladder, and marketing strategy — or stay in sketch until killed or ported.

This lane does **not** replace the factory default: **JUCE + VST3 + CLAP** (see [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md) tracks A–C).

## When to use HISE vs JUCE factory

| Use HISE sketch lane | Use JUCE / Cursor factory |
|----------------------|---------------------------|
| Rompler, sample maps, HLAC, multi-mic kits | Junova-X, TR-808 circuit DSP, JD Upgraded |
| Fast “does it sell?” VSTi on Windows | CLAP MVP, Linux CI, pluginval in monorepo |
| Antigravity owns full loop on your PC | GitHub PRs, WO-2026-001…003 style acceptance |
| Marketing wants a **sampler** story | Marketing wants **analog poly** or **808 circuit** story |

## Agent roles

| Role | Agent / human | Responsibility |
|------|----------------|----------------|
| **Sketch implementer** | Antigravity (+ HISE tools) | HiseScript, XML preset, `export_ci` + `batchCompile.bat`, local `.vst3` |
| **Ship implementer** | Cursor Cloud Agent | JUCE products in this repo; only touches HISE after an explicit **port WO** |
| **Business Planner** | Human + Airtable | Approve new `product_id`, family, price tier, phase; cap parallel sketches |
| **Marketing** | Human + briefs | Positioning, name, landing, demo policy; blocks “silent” SKUs |
| **Advisory** | Grok Plugin team | DSP/UI ideas; prefix WOs `[Plugin][HISE]` when Airtable-ready |

Antigravity may own this lane **end-to-end** on a Windows workstation. Cursor WIP limits (max 2 implementation WOs) apply to **JUCE/repo** work only unless Factory Manager extends policy in Airtable.

## Lifecycle (gates)

```text
Idea / brief (Marketing + Planner)
        │
        ▼
Sketch (Antigravity) ──► local .vst3 + 1-pager (sonic + UI)
        │
        ├── Kill ──► status Killed in Airtable
        ├── Ship as HISE product ──► SKU row + Windows VST3 GTM (no CLAP unless later port)
        └── Promote to JUCE ──► new WO: port acceptance + CLAP if required
```

**Promotion to “Product” in Airtable** requires:

1. `product_id`, name, `division: Plugin`, `family_id`, `price_tier`, `ladder_tier`
2. Marketing one-liner (`sonic_problem`) and launch tier ($29 demo policy if plugin)
3. Business Planner sign-off (fits line, not duplicating Junova/TR-808/JD)
4. Acceptance criteria explicit about **VST3-only** vs **future JUCE port**

## Technical architecture (two layers)

```text
┌─────────────────────────────────────────────────────────────┐
│  Antigravity agent (local)                                   │
│  1. Spec (brief / Airtable WO)                               │
│  2. write_hisescript → project/Scripts/*.js                  │
│  3. compile_hise_vst → export_ci + batchCompile.bat         │
└──────────────────────────────┬──────────────────────────────┘
                               ▼
┌─────────────────────────────────────────────────────────────┐
│  HISE Engine                                                 │
│  XmlPresetBackups/*.xml · Scripts/*.js · generated C++/JUCE  │
│  Output: Binaries/Compiled/…/*.vst3 (typical)                │
└─────────────────────────────────────────────────────────────┘
```

### System requirements (Windows)

- Visual Studio 2022 — **Desktop development with C++**
- HISE standalone built from source (or release); **HISE source path** for `-h:` / `set_hise_folder`
- Steinberg VST3 SDK (HISE tools/SDK layout)
- Base project saved once in HISE (initializes `XmlPresetBackups/`)

### Reliable CLI sequence (CI mode)

Use **project-relative** XML paths in `export_ci`. Typical flow:

```batch
set HISE_EXE=C:\HISE-develop\projects\standalone\Builds\VisualStudio2022\x64\Release\App\HISE.exe
set HISE_SRC=C:\HISE-develop
set PROJECT=C:\HISE_Projects\MyInstrument

"%HISE_EXE%" -h:%HISE_SRC%
"%HISE_EXE%" set_project_folder -p:"%PROJECT%"
"%HISE_EXE%" export_ci "XmlPresetBackups\MyInstrument.xml" -t:instrument -p:VST3 -a:x64
call "%PROJECT%\Binaries\batchCompile.bat"
```

Flags: `-t:instrument|effect|standalone`, `-p:VST3` (prefer VST3 over legacy VST).  
`export_ci` alone often **does not** finish linking — always run **`batchCompile.bat`** and check its exit code.

### Antigravity tool contract (minimum)

| Tool | Behavior |
|------|----------|
| `write_hisescript` | Write UTF-8 to `{PROJECT}/Scripts/{filename}` |
| `compile_hise_vst` | `set_project_folder` → `export_ci` → `batchCompile.bat`; return log tail + artifact path |
| `compile_script` (recommended) | HISE `compile_script` before full export |
| `clean` (recommended) | HISE `clean` when iterating failed exports |

Store `HISE_EXE`, `HISE_SRC`, `PROJECT_DIR` in env — never commit API keys or PATs.

Reference implementation pattern (Antigravity): local Python tools wrapping `subprocess` on `HISE.exe`; system instructions: HiseScript + UI callbacks + export discipline.

## Repository layout (when sketches land in git)

Optional import path (not required for first sketches):

```text
hise-sketch/
├── ARCHITECTURE.md          ← lane index when projects exist
├── README.md
└── <ProductName>/           ← one folder per promoted or active sketch
    ├── XmlPresetBackups/
    ├── Scripts/
    └── docs/brief.md          ← Marketing + Planner fields
```

Sketches may live **only on disk** until Marketing approves a git import.

## Airtable

- Seed template: [`disklordz/airtable/seed/hise-sketch-lane-2026.json`](../disklordz/airtable/seed/hise-sketch-lane-2026.json)
- Work order title prefix: **`[Plugin][HISE]`**
- Suggested family for romplers: **`DL-FAMILY-ROMPLER`** (create in bootstrap if missing; else `DL-FAMILY-DIGITAL-SAMPLER` temporarily)

## Related

- [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md) — tracks A–C (JUCE factory)
- [Junova-X/REPO_HANDOFF.md](../Junova-X/REPO_HANDOFF.md) — **not** HISE
- HISE CLI glossary: https://docs.hise.dev/glossary/command-line-tool.html
