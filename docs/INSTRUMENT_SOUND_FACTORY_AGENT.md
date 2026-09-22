# Instrument & Sound Factory Agent — enterprise design (planning)

**Status:** Planning / Phase 0 — no production worker fleet yet.  
**Owner lane:** Disklordz product factory + JUCE/HISE instrument tracks.  
**Read first:** [ARCHITECTURE.md](../ARCHITECTURE.md) · [DISKLORDZ_ILLUGEN_RESEARCH.md](DISKLORDZ_ILLUGEN_RESEARCH.md) · [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md)

---

## 1. Mission

Build a **production-grade, mostly unattended** Cursor Cloud agent system that can:

| Capability | Output artifacts |
|------------|------------------|
| **Audio generation** (parametric → future model gateway) | WAV + provenance + `DISKLORDZ_*_MANIFEST` |
| **Batch creation** | SKU folders, variation batches, ZIP packs |
| **Sound design** | Processing chains, layer maps, brief → spec JSON |
| **Preset design** | JD Upgraded / Wave909 APVTS presets, factory programs |
| **Multisample maps** | SFZ, internal zone tables (JD ROM / HISE maps) |
| **MPC Software keygroup programs** | Intermediate **KeygroupSpec JSON** → MPC export (Phase 2) |
| **Reference matching** | Spectral/ loudness / transient reports vs user-supplied refs |
| **Research** | Song/track discovery (metadata only) + user-provided audio for analysis |
| **JUCE DSP** | C++ changes with `run_business.py --profile ci` gate |
| **HISE sketch** | Git handoff to Antigravity, not remote HISE in Cloud VM |

**Default mode:** **Automated worker** (queue-driven). **Interactive mode:** same orchestrator, human approves writes (MPC project save, deploy, Zapier/Airtable).

---

## 1.1 Three runtimes (same agent, different superpowers)

One **logical agent** (same docs, skills, job JSON, manifests). You run it wherever the work needs to happen:

| Runtime | Where | Best for | Cannot do (today) |
|---------|--------|----------|-------------------|
| **Cursor Cloud** | Linux VM in Cursor | Git PRs, JUCE CI, Python/Node batch WAV, SFZ, RAG, spectral QA | MPC `.xpj` build, HISE export, your local MPC hardware smoke test |
| **VS Code / Cursor IDE (local)** | Your Mac or PC | **MPCTK** (`mpctk` CLI/GUI), drag WAVs to MPC, reference files on disk, faster iteration | Full monorepo JUCE CI unless you install toolchain |
| **Antigravity** | Your Windows + HISE stack | HISE romplers, `batchCompile.bat`, optional **MPCTK** on same machine | Becomes source of truth for JUCE ship — hand off port WOs to Cloud |

**Handoff pattern:** Cloud finishes **WAV + manifest (+ SFZ)** → commits or uploads artifacts → you (or Antigravity) run **MPCTK** or HISE on local runtime → optional note back in git (`outbox/` or PR comment).

Add optional field on jobs: `"runtimeHint": "cloud" | "local" | "antigravity" | "split"` (`split` = Cloud generates, local exports MPC).

---

## 1.2 Plain-language glossary (planning)

| Jargon | What it means for you |
|--------|------------------------|
| **Worker trigger** | **What starts the agent when you are not chatting with it.** Examples: you click “Run Cloud Agent” with a task; you drop a file in `sound-factory/jobs/` and tell the agent to run it; later: a timer or Airtable row auto-starts a run. **Planning default:** you explicitly start runs (simplest). We add “wake up on schedule / new WO” only when you want hands-off nights. |
| **MPC export path** | **How WAVs become a playable MPC project.** Preferred: your **[MPC Sample Toolkit (MPCTK)](https://github.com/samuelfreemanjobs-hash/MPC-Sample-Toolkit)** — validated `.xpj` read/write, chromatic pad banks, WAV injection, CLI + macOS GUI, hardware-tested. Fallback: clicking in MPC Software (Bytebot) only if MPCTK cannot cover a case. There is no separate `mpc-agent` repo in your GitHub; MPCTK *is* the MPC automation layer for now (future: thin “agent wrapper” skill that calls `mpctk`). |
| **Reference policy** | **What audio the agent is allowed to listen to and copy.** Web search = facts (BPM, key, gear). **Matching a song’s sound** = analyze **files you provide** (WAV/MP3 you drop in or attach), not ripped streaming audio. Optional later: a licensed folder of stems you own. |
| **Cloud image split** | **Two different Cloud VM setups:** (A) heavy image with JUCE compile for plugin work; (B) light image with only Node/Python for drum-kit batches (faster/cheaper). Planning note: one image is fine until batch jobs feel slow or expensive — not a decision you need now. |
| **First vertical slice** | The **first end-to-end product story** we implement (e.g. kit → ZIP vs kit → SFZ → MPC). You will pick this after planning. |

---

## 2. Fit in this monorepo

```text
                    ┌─────────────────────────────────────┐
                    │  Orchestrator (Cloud / VS Code / AG)   │
                    └──────────────┬──────────────────────┘
           ┌───────────────────────┼───────────────────────┐
           ▼                       ▼                       ▼
   disklordz/website          sound-factory/           Source/ + Wave909/
   generation + API           Python batch CLI         JUCE DSP + presets
           │                       │                       │
           ▼                       ▼                       ▼
   manifest.ts schemas      generate_kit.py          vst-testing-ops CI
           │                       │                       │
           └───────────┬───────────┴───────────┬───────────┘
                       ▼                       ▼
              KeygroupSpec / SFZ / HLAC map   Golden WAV + SpectralDiff
                       │
                       ▼
              daw-inbox / ZIP / Supabase storage (when live)
```

Existing contracts to **extend**, not fork:

- `DISKLORDZ_DRUM_KIT_MANIFEST` · `DISKLORDZ_PRODUCT_PACK_MANIFEST` — [disklordz/website/src/lib/manifest.ts](../disklordz/website/src/lib/manifest.ts)
- `GenerationSpec` — [disklordz/website/src/lib/generation/generation-spec.ts](../disklordz/website/src/lib/generation/generation-spec.ts)
- Plugin QA — `python3 vst-testing-ops/run_business.py --profile ci`
- RAG lane vocabulary — [disklordz/rag/](../disklordz/rag/)

---

## 3. Agent topology (orchestrator + subagents)

Cursor **subagent types** map to factory roles (single orchestrator delegates; workers do not merge PRs without QA).

| Role | Subagent | When |
|------|----------|------|
| **Orchestrator** | Main Cloud Agent + skill `instrument-orchestrator` | WO intake, job DAG, PR |
| **Repo exploration** | `explore` | Find modules, manifest paths, preset loaders |
| **DSP / bugfix** | `debug` | Non-trivial audio bugs, NaNs, denormals |
| **JUCE CI failure** | `ci-investigator` | Red `build.yml` / pluginval |
| **Parallel batch attempts** | `best-of-n-runner` | N variation seeds / export retries in worktrees |
| **Reference video** | `videoReview` | User attaches performance/reference MP4 |
| **MPC `.xpj` build** | **Local / Antigravity** + MPCTK CLI/GUI | Chromatic/keygroup projects — **not** Cloud VM |
| **MPC UI fallback** | Bytebot / `computerUse` | Only if MPCTK lacks a feature |
| **Optional deep review** | `bugbot` / `security-review` | On explicit user request |

**Human roles (gates):** Business Planner + Marketing (SKU), Factory Manager (WIP cap), legal for **reference audio** (see §8).

---

## 4. Skills (repo-local)

Installed under [`.cursor/skills/`](../.cursor/skills/) — agents **read the SKILL.md** for the task:

| Skill | Purpose |
|-------|---------|
| `instrument-orchestrator` | Routing, modes, definition of done |
| `audio-generation-batch` | Parametric + API batch, manifest discipline |
| `sound-design-reference-match` | Analysis CLI, ref workflow, evidence |
| `sfz-multisample-map` | SFZ opcodes, zone layout, round-robins |
| `mpc-keygroup-program` | KeygroupSpec JSON → export pipeline (phased) |
| `preset-design-factory` | JD / Wave909 / HISE XML presets |
| `juce-dsp-plugin-ci` | CMake, golden WAV, pluginval |
| `hise-antigravity-handoff` | Bridge scripts, inbox/outbox |

**Marketplace / MCP (enable in Cursor settings, not in git):**

| Need | Tooling |
|------|---------|
| Deploy / env | Vercel, Supabase skills |
| WO automation | Zapier → Airtable (writes need confirmation) |
| Timer / PR / CI subscriptions | `cursor-subscriptions` MCP |
| Web research | `WebSearch`, `WebFetch` (cursor namespace) |
| Slack ops | Native Slack MCP |

---

## 5. Job model (automated worker)

Phase 1: **file-based queue** in git (no new infra). Phase 2: Supabase `factory_jobs` (see ILLUGEN job sketch in RAG doc).

### 5.0 How jobs get started (worker triggers — simplified)

| Trigger | Who uses it | Status |
|---------|-------------|--------|
| **Manual** — you open Agent (Cloud or VS Code) and paste goal + job path | Everyone | **Default for planning** |
| **Manual** — `@instrument-orchestrator` + “run `jobs/foo.json`” | Everyone | Same as above |
| **Git** — PR or comment with `factory-run: jobs/foo.json` | Cloud | Phase 1 optional |
| **Timer** — nightly “generate SKU X” | Cloud subscriptions MCP | Phase 2 optional |
| **Airtable WO** — new row kicks Cloud Agent | Zapier + API | Phase 2 optional |

You do **not** need to pick a trigger now. Default = **you start the agent** with a job file or structured prompt.

### 5.1 Work order → job DAG

```json
{
  "workOrderId": "WO-SF-001",
  "productId": "DL-ROMPLER-001",
  "mode": "worker",
  "brief": {
    "prompt": "1997 Memphis phonk bell stack",
    "presetId": "DL006",
    "generationSpec": { "mode": "one_shot", "bpm": 92, "key": "F# minor" }
  },
  "pipeline": [
    { "step": "rag.retrieve", "lane": "DL006" },
    { "step": "generate.batch", "variations": 8 },
    { "step": "qa.spectral", "tool": "SpectralDiff" },
    { "step": "map.sfz" },
    { "step": "export.mpc_keygroup", "optional": true },
    { "step": "package.zip" }
  ],
  "acceptance": {
    "manifestFormat": "DISKLORDZ_PRODUCT_PACK_MANIFEST",
    "maxLufsDelta": 2.0,
    "ciProfile": null
  }
}
```

### 5.2 Worker loop (any runtime)

1. **Start** via trigger (default: manual).  
2. Load orchestrator skill + product `ARCHITECTURE.md`.  
3. Execute DAG steps allowed on this runtime (`runtimeHint`); skip or defer MPC/HISE steps to local/Antigravity with a clear handoff file.  
4. Write artifacts to **`disklordz/sound-factory/out/<jobId>/`** (gitignored) or storage when Supabase live.  
5. **Cloud:** open draft PR with manifest + QA logs. **Local/Antigravity:** run MPCTK/HISE; report paths or push branch.  
6. Mark WO Done only after human merge (agent never auto-merge).

**Later:** optional poll/timer for unattended Cloud runs.

---

## 6. Format strategy

### 6.1 SFZ (Phase 1 — open, automatable)

- Canonical map: **`InstrumentMapSpec`** (internal JSON) → emit `.sfz` + `samples/` tree.  
- Skills document opcode subset: `sample`, `key`, `lokey`, `hikey`, `pitch_keycenter`, `volume`, `pan`, `loop_mode`, `cutoff`, envelope opcodes as needed.  
- HISE lane can import SFZ or mirror the same spec.

### 6.2 MPC Sample projects (MPCTK — primary)

**Tool:** [MPC-Sample-Toolkit (MPCTK)](https://github.com/samuelfreemanjobs-hash/MPC-Sample-Toolkit) — gzip `.xpj` read/write, chromatic pad banks, scale layouts, WAV injection, `mpctk` CLI, PySide6 GUI, validated on **physical MPC Sample** hardware.

**Integration plan (no vendoring into Instruments until WO):**

1. Factory agent emits **source WAV(s)** + `manifest.json` (and optional **InstrumentMapSpec** for roots/transpose).  
2. On **local VS Code** or **Antigravity** runtime: `mpctk` (or GUI) with structural XPJ template + source/target roots.  
3. Output `.xpj` + `_[ProjectData]` package → user loads on hardware.  
4. Optional WO: git submodule `tools/MPC-Sample-Toolkit` or documented clone + venv in `AGENTS.md`.

**KeygroupSpec JSON** remains the in-repo interchange format so SFZ and MPC steps share the same zone list; MPCTK is the **exporter**, not manual UI clicking.

**Fallback:** Bytebot / MPC Software UI only for gaps MPCTK does not cover yet.

Orchestrator asks before overwriting user MPC project directories.

### 6.3 JUCE multisample (JD Upgraded)

- Zone metadata from [docs/ROM.md](ROM.md) · `GenerateCleanroomRom` · `RomBank::selectForNote`.  
- Agent tasks: tune zone roots, pair with `FactoryPatchLibrary`, refresh golden if intentional.

### 6.4 Presets

| Stack | Path |
|-------|------|
| JD Upgraded | APVTS + factory programs · [docs/PRESETS.md](PRESETS.md) |
| Wave909 | [Wave909/ARCHITECTURE.md](../Wave909/ARCHITECTURE.md) |
| HISE | XmlPresetBackups — handoff only |

---

## 7. Audio generation lanes

| Lane | Now | Next |
|------|-----|------|
| **Web factory** | `disklordz/website/src/lib/generation/*` | Async jobs + credits (WO-SAAS-007+) |
| **Python batch** | `disklordz/sound-factory/scripts/` | Same `GenerationSpec` as TS |
| **Plugin render** | `tools/OfflineRender` | Stem export for multisample source |
| **External model** | — | Gateway behind same manifest + provenance enum |

Batch skill requires: **SHA-256 per WAV**, `sourceId`, `provenance` enum extension in manifest when adding engines.

---

## 8. Reference listening & “find songs online”

**In plain terms:** the agent can Google *about* a track (tempo, key, machines used). To *sound like* something, you give it **audio files you are allowed to use** — a exported WAV, a sample pack file, or a recording you made — not “pull from Spotify.”

**Legal / safety baseline**

- Agent may **search the public web** for song titles, BPM, key, gear lists, interviews (facts).  
- Agent must **not** download copyrighted audio from streaming/YouTube unless the **user supplies files** or confirms licensed assets.  
- Reference **analysis** runs on user-provided WAV/MP3 in `disklordz/sound-factory/refs/` or WO attachments.

**Technical pipeline (Cloud VM)**

1. **Ingest:** user drops ref in `sound-factory/refs/` or attaches to WO.  
2. **Analyze (Phase 1 CLI — to implement):** `python3 disklordz/sound-factory/scripts/analyze_reference.py` → JSON report (LUFS, centroid, transient rate, chroma sketch, optional mel PNG).  
3. **Match:** adjust `GenerationSpec` + factory params; regenerate batch; `SpectralDiff` vs ref slice.  
4. **Video:** `videoReview` subagent for user-supplied performance video; no audio extraction without permission.

**Evidence for acceptance:** report JSON + spectrogram PNG + A/B WAV clips in PR.

---

## 9. QA & production gates

| Gate | Command / tool |
|------|----------------|
| TS / web factory | `cd disklordz/website && npm ci && npm run build` |
| Python batch | `python3 disklordz/sound-factory/scripts/generate_kit.py ...` |
| JUCE DSP | `python3 vst-testing-ops/run_business.py --profile ci` |
| Spectral regression | `tools/SpectralDiff` (when maps share render path) |
| Golden WAV | `tests/golden/verify_golden.sh` — refresh only intentionally |

Worker jobs **fail closed**: bad manifest schema, missing SHA-256, or QA over threshold → no ZIP publish.

---

## 10. Environment extensions (future `.cursor/environment.json`)

**Plain terms:** Cloud VMs are pre-installed Linux boxes. “Split image” just means we might offer a **slim** box for drum batches and a **fat** box for JUCE — defer until needed.

| Profile | Install focus | Runtime |
|---------|---------------|---------|
| **plugin** (today) | JUCE + gcc-12 | Cloud |
| **factory** (planned) | Node + Python scipy/ffmpeg for analysis | Cloud |
| **local** | MPCTK venv, MPC template XPJ, optional HISE | VS Code / Antigravity |

Current Cloud install is JUCE-only. Planned additive packages for factory profile:

- `ffmpeg`, `sox`, `python3-scipy`, `librosa` (or lightweight scipy-only features first)
- **MPCTK:** local/Antigravity only (macOS GUI / hardware validation), not Cloud install
- Optional: `yt-dlp` **disabled by default** until legal workflow documented

---

## 11. Implementation phases (backlog)

| Phase | Deliverable | WO prefix |
|-------|-------------|-----------|
| **0** | This doc + repo skills + RAG corpus entries | — |
| **1** | `InstrumentMapSpec` + SFZ emitter + unit tests | `WO-SF-010` |
| **1** | `analyze_reference.py` + report schema | `WO-SF-011` |
| **1** | Batch driver: read job JSON, call `generate_kit.py` / API | `WO-SF-012` |
| **2** | KeygroupSpec schema + MPCTK handoff doc / optional submodule | `WO-SF-020` |
| **2** | Supabase `factory_jobs` + worker subscription | `WO-SF-021` |
| **3** | Async generation gateway alignment (ILLUGEN 03–07) | `WO-SAAS-007+` |
| **3** | HISE import of SFZ / map spec via handoff | `[Plugin][HISE]` |

---

## 12. Structured prompt template (worker tasks)

```markdown
## Goal
Run factory job WO-SF-xxx unattended until draft PR.

## Context
- Read: docs/INSTRUMENT_SOUND_FACTORY_AGENT.md, .cursor/skills/instrument-orchestrator/SKILL.md
- Job file: disklordz/sound-factory/jobs/<id>.json

## Requirements
1. Execute pipeline steps in order; fail closed on QA.
2. Extend manifest provenance if new engine used.
3. No secrets; no copyrighted ref downloads.

## Success criteria
- [ ] Artifacts under sound-factory/out/<id>/
- [ ] manifest.json valid against TS types or JSON schema
- [ ] Evidence: analyze report and/or CI log
- [ ] Draft PR; do not merge
```

---

## Related docs

- [DISKLORDZ_SAAS_AGENT_LANES.md](DISKLORDZ_SAAS_AGENT_LANES.md)
- [RAG_AND_INTELLIGENT_AUTOMATION.md](RAG_AND_INTELLIGENT_AUTOMATION.md)
- [MPC-Sample-Toolkit](https://github.com/samuelfreemanjobs-hash/MPC-Sample-Toolkit) — `.xpj` generation (local/Antigravity)
- [BYTEBOT_SETUP.md](BYTEBOT_SETUP.md) — MPC UI fallback only
- [HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md)
