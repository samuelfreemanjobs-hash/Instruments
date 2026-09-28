# Junova-X → Instruments monorepo handoff

**Canonical path in this repo:** `vst-juno106/` (product root).  
**Status (honest):** scaffold + Main/Diag UI + DSP modules exist · **not DAW-tested** · **no public remote committed yet** · Windows host build required for GTM.

## Identity

| Field | Value |
|-------|--------|
| Product | **Junova-X** |
| VST3 bundle | `JunovaX.vst3` |
| Plugin ID | `JnvX` |
| Manufacturer | `SmFr` |
| Stack | **iPlug2** + Steinberg **VST3 SDK** (not JUCE) |
| Internal folder | `plugin/Juno106/` (name retained on purpose) |

## What belongs in the repo

```text
vst-juno106/
├── REPO_HANDOFF.md          ← this file
├── plugin/
│   ├── Juno106/             ← code, .sln, DSP, UI
│   └── scripts/
│       ├── fetch-deps.sh
│       └── (see BUILD_WINDOWS.md)
├── BUILD_WINDOWS.md
├── docs/                    ← Spec, DSP, UI, QA
├── gtm/                     ← store one-pagers, pricing copy
└── .gitignore               ← product-local ignores (see below)
```

- **iPlug2** as **git submodule** (not the full downloaded deps tree).
- Spec / DSP / UI / QA docs + GTM/store one-pagers.

## Keep out of git

- Fetched **VST3 SDK** tree (via `fetch-deps.sh`)
- **IGraphics** prebuilts
- `build-win/`, `*.vst3` binaries
- `node_modules/`

## Separate repo (recommended)

**Landing:** `junova-x-landing` — Vite site, **$29 → $49** copy already wired. Do not merge marketing site into this plugin tree.

## GTM working plan

- Launch price ladder: **$29** early → **$49** standard
- **Windows demo build required** at launch (primary buyer path)
- README must state: not DAW-validated until QA matrix is run

## Publish checklist (GitHub)

1. Create remote **`junova-x`** (or monorepo path `vst-juno106/` on `Instruments` — **preferred: monorepo folder** below).
2. Commit with **iPlug2 submodule** pointer.
3. Clone: `git clone --recurse-submodules …`
4. `plugin/scripts/fetch-deps.sh`
5. Build: **Juno106-vst3** · **Release \| x64**

**Blocker:** GitHub SCM / PAT must be connected to create remote or push submodule history.

### Monorepo integration (Disklordz)

When importing into `samuelfreemanjobs-hash/Instruments`:

1. Land `vst-juno106/` as above (submodule + scripts + docs).
2. Add WO in Airtable: `[Plugin][Junova-X] First Release x64 VST3 host smoke`.
3. CI later: Windows workflow only for this path (do not mix with JD Upgraded CMake CI).

## Agent / Grok boundaries

- **Grok Plugin team:** specs, DSP notes, UI wireframes, QA matrices.
- **Cursor Cloud Agent:** implementation PRs under `vst-juno106/`.
- **JD Upgraded** (`Source/`) is a **different stack (JUCE)** — no shared DSP without an explicit WO.

## Related

- [docs/DISKLORDZ_PLUGIN_TRACKS.md](../docs/DISKLORDZ_PLUGIN_TRACKS.md)
- TR-808 sibling track: [vst-tr808/README.md](../vst-tr808/README.md)
