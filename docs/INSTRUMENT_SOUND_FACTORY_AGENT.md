# Instrument & Sound Factory Agent — enterprise design (planning)

**Status:** Planning / Phase 0 — no production worker fleet yet.  
**Owner lane:** Disklordz product factory + JUCE/HISE instrument tracks.  
**Read first:** [ARCHITECTURE.md](../ARCHITECTURE.md) · [DISKLORDZ_ILLUGEN_RESEARCH.md](DISKLORDZ_ILLUGEN_RESEARCH.md) · [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md)

### Locked decisions (product owner)

| Topic | Decision |
|-------|----------|
| **Who starts runs** | **You** manually **or** **PM Agent / Airtable** plans the WO and kicks the factory agent |
| **MPC deliverable** | Multisamples → **MPC Keygroup program** via your **Cursor MPC-agent** + **MPCTK** |
| **Reference audio** | **YouTube allowed** — find real tracks, download **short segments for analysis/timbre match**; never commit or ship ref audio in products |
| **Cloud VM** | **Heavy image only** (JUCE + factory tools); no slim split for now |

---


## 1. Mission

Build a **production-grade, mostly unattended** Cursor Cloud agent system that can:

| Capability | Output artifacts |
|------------|------------------|
| **Audio generation** (parametric → future model gateway) | WAV + provenance + `DISKLORDZ_*_MANIFEST` |
| **Batch creation** | SKU folders, variation batches, ZIP packs |
| **Sound design** | Processing chains, layer maps, brief → spec JSON |
| **Preset design** | JD Upgraded / Wave909 APVTS presets, factory programs |
| **Multisample maps** | **SFZ required** on every project + internal zone tables |
| **MPC Keygroup programs** | Multisample zones → **Cursor MPC-agent** → MPCTK `.xpj` |
| **Reference matching** | YouTube + spectral metrics; timbre-target factory params |
| **Research** | Find tracks on YouTube/web; analysis segments in gitignored cache |
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
| **Worker trigger** | **What starts a run:** (1) **You** start Cloud/VS Code Agent with a job file, or (2) **PM Agent / Airtable** creates the WO and triggers the implementation agent. See skill `pm-airtable-factory-trigger`. |
| **MPC export path** | **Multisamples → Keygroup program:** your **Cursor MPC-agent** (specialist) + **[MPCTK](https://github.com/samuelfreemanjobs-hash/MPC-Sample-Toolkit)**. Skill: `cursor-mpc-agent`. |
| **Reference policy** | **YouTube enabled** for timbre: search → short yt-dlp segment → analyze → tune factory; refs stay in gitignored cache, not in SKU ZIPs. Skill: `sound-design-reference-match`. |
| **Cloud image** | **Heavy only** — JUCE + ffmpeg/yt-dlp when WO lands; no separate slim Cloud image for now. |
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
| `mpc-keygroup-program` | Multisample → MPC Keygroup program |
| `cursor-mpc-agent` | Delegate to your Cursor MPC-agent + MPCTK |
| `pm-airtable-factory-trigger` | WO / PM Agent → job JSON → Cloud run |
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

### 5.0 How jobs get started (worker triggers)

| Trigger | Who | Status |
|---------|-----|--------|
| **Manual** — you run Cloud or VS Code Agent with goal + `jobs/<id>.json` | You | **Active** |
| **PM Agent / Airtable** — WO row → issue → job JSON → Cloud Agent | Planner / PM Agent | **Active** (automation depth Phase 2) |
| **Timer** — nightly SKU batch | subscriptions MCP | Optional later |
| **Git comment** — `factory-run: jobs/foo.json` | Cloud | Optional |

Skill: `.cursor/skills/pm-airtable-factory-trigger/SKILL.md`.

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
    { "step": "export.mpc_keygroup", "delegate": "cursor-mpc-agent", "required": true },
    { "step": "reference.youtube", "url": "https://...", "segmentSec": 60 },
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

### 6.2 MPC Keygroup programs (MPC-agent + MPCTK)

**Requirement:** multisample sets must land as a **Keygroup program**, not drum-one-shots only.

| Layer | Role |
|-------|------|
| **Instrument orchestrator** | WAV render, `instrument-map.json`, manifest |
| **Your Cursor MPC-agent** | Keygroup layout, QA, invokes MPCTK / extensions |
| **[MPCTK](https://github.com/samuelfreemanjobs-hash/MPC-Sample-Toolkit)** | `.xpj` + `_[ProjectData]` write path |

Handoff directory: `disklordz/sound-factory/out/<jobId>/` — see skill `cursor-mpc-agent`.

**InstrumentMapSpec / zone JSON** is shared with SFZ so one map feeds HISE, SFZ, and MPC.

**Runtime:** MPC-agent + MPCTK on **VS Code or Antigravity**; Cloud completes handoff + `HANDOFF_MPC.md` when `runtimeHint` is `split`.

**Fallback:** Bytebot / MPC UI only if MPC-agent + MPCTK gap is documented in the WO.

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

## 8. Reference listening (YouTube + uploads)

**Owner policy:** for timbre matching, agents **find real songs on YouTube**, download **short segments for analysis**, and tune the factory. Skill: `sound-design-reference-match`.

**Guardrails (still required)**

- Cache under `disklordz/sound-factory/refs/youtube-cache/` — **gitignored**.  
- **Do not** commit reference audio or include it in customer ZIPs / store assets.  
- Prefer identifiable segments (user names hook/intro); default cap ~60s for analysis.  
- PR evidence = `reference-report.json` + spectrograms + **synthetic** A/B WAVs only.

**Technical pipeline (heavy Cloud VM — WO-SF-011)**

1. **Resolve:** WebSearch / user title → YouTube URL.  
2. **Extract:** `yt-dlp` + `ffmpeg` → temp WAV in youtube-cache.  
3. **Analyze:** `analyze_reference.py` → LUFS, centroid, transients, mel PNG.  
4. **Match:** update `GenerationSpec` / multisample render params → regenerate zones.  
5. **Optional:** user uploads in `sound-factory/refs/` or `videoReview` on attached MP4.

**Legal:** private factory use under owner direction; agents remain fail-closed on shipping third-party audio.

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

## 10. Cloud environment (heavy image only)

Single **heavy** Cloud image: keep today’s JUCE/gcc-12 install; extend the same image when WOs land:

- `ffmpeg`, `yt-dlp` (YouTube analysis lane), Python scipy/librosa for metrics  
- Optional Node for `disklordz/website` factory parity on same agent  

**Not on Cloud:** MPCTK GUI, MPC-agent keygroup export, HISE — **local / Antigravity** only.

**Local profile:** MPCTK venv, your **Cursor MPC-agent** rules/skills, MPC template `.xpj`.

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
