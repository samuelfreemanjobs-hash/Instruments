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

## 2. Fit in this monorepo

```text
                    ┌─────────────────────────────────────┐
                    │  Orchestrator (Cloud Agent + WO)     │
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
| **MPC UI / desktop** | `computerUse` or **Bytebot local** | MPC Software clicks — **not** in Cloud VM |
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

### 5.2 Worker loop (Cloud Agent)

1. Poll: Airtable WO / GitHub label `factory-worker` / `cursor-subscriptions` timer.  
2. Load orchestrator skill + product `ARCHITECTURE.md`.  
3. Execute DAG; commit artifacts to **`disklordz/sound-factory/out/<jobId>/`** (gitignored) or storage backend when Supabase live.  
4. Open **draft PR** with manifest + QA logs + walkthrough (spectrogram PNG / short demo video).  
5. Mark WO Done only after human merge (agent never auto-merge).

**Idle behavior:** subscribe to queue; optional nightly batch from `manifest.json` SKUs.

---

## 6. Format strategy

### 6.1 SFZ (Phase 1 — open, automatable)

- Canonical map: **`InstrumentMapSpec`** (internal JSON) → emit `.sfz` + `samples/` tree.  
- Skills document opcode subset: `sample`, `key`, `lokey`, `hikey`, `pitch_keycenter`, `volume`, `pan`, `loop_mode`, `cutoff`, envelope opcodes as needed.  
- HISE lane can import SFZ or mirror the same spec.

### 6.2 MPC Software keygroup (Phase 2 — proprietary)

- Do **not** binary-edit `.xpm` blindly in v1.  
- **KeygroupSpec JSON** (zones, root, tune, filter, envelope, sample path, group mute/solo rules) validated by schema.  
- Export paths (pick one in implementation WO):
  - **A)** Documented MPC Software UI steps + Bytebot/computerUse on Windows host.  
  - **B)** Reverse-engineered text/XML fragment if MPC exposes it in project folder (research WO).  
  - **C)** Akai official export from MPC Beats / documented interchange if available.

Orchestrator treats MPC export as **optional step** with explicit user confirmation before writing project files.

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

**Legal / safety baseline**

- Agent may **search the public web** for song titles, BPM, key, gear lists, interviews (facts).  
- Agent must **not** download copyrighted audio from streaming/YouTube unless the **user supplies files** or confirms licensed assets.  
- Reference **analysis** runs on user-provided WAV/MP3 in repo or artifact upload.

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

Current install is JUCE-only. Planned additive packages for sound factory worker:

- `ffmpeg`, `sox`, `python3-scipy`, `librosa` (or lightweight scipy-only features first)
- Optional: `yt-dlp` **disabled by default** until legal workflow documented

Separate **web-only** Cloud image: Node 20 + `disklordz/website` build without full JUCE (faster kit batches).

---

## 11. Implementation phases (backlog)

| Phase | Deliverable | WO prefix |
|-------|-------------|-----------|
| **0** | This doc + repo skills + RAG corpus entries | — |
| **1** | `InstrumentMapSpec` + SFZ emitter + unit tests | `WO-SF-010` |
| **1** | `analyze_reference.py` + report schema | `WO-SF-011` |
| **1** | Batch driver: read job JSON, call `generate_kit.py` / API | `WO-SF-012` |
| **2** | KeygroupSpec schema + MPC export research spike | `WO-SF-020` |
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
- [BYTEBOT_SETUP.md](BYTEBOT_SETUP.md) — MPC desktop automation
- [HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md)
