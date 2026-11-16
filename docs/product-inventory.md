# Disklordz product inventory (sellability audit)

**Audit date:** 2026-11-10  
**Scope:** `main` at `90cfb560`, open/draft/merged `cursor/*` branches and linked PRs (read-only; no product code changed).  
**Shop target:** WooCommerce on Disklordz.com.

Evidence tags: **[executed]** = ran commands/generators in Cloud VM this audit; **[inspected]** = read repo/PR metadata; **[assumed]** = not verified in this run.

---

## Executive summary

| Category | Phonk-adjacent kits **with committed audio** on `main` | Regenerable phonk kits (scripts on branch) |
|----------|--------------------------------------------------------|--------------------------------------------|
| Count | **7** named product surfaces (5 SaaS stubs + 5 root loops + 5 SparkLE sprints — overlap: only **1** stub is phonk-branded) | **Unbounded** banks/loops via Lofi-12 factory (PR [#69](https://github.com/samuelfreemanjobs-hash/Instruments/pull/69)) |

Honest bottom line: the **strongest phonk commerce path** is merge **PR #69** (`cursor/lofi12-phonk-factory-f8f8`), run one-shot + loop factories, QA listen, then ZIP with manifest + EULA. What's on **`main` today** is mostly **v0 procedural stubs** (sellable only with clear “synthetic preview” positioning) plus **five repo-root loop WAVs of unknown provenance** (do not ship until cleared).

---

## 1. Phonk drum kits and factories (priority)

### 1.1 Kit inventory at a glance

| # | Product / kit name | Type | Location | Committed audio? | Slots / counts | Format |
|---|-------------------|------|----------|------------------|----------------|--------|
| A | **Midnight Circuit** (drift phonk SaaS preset) | 6× one-shot kit | `main`: `disklordz/website/public/samples/midnight-circuit/` | **Yes** — 6 WAV | kick, snare, hat_closed, hat_open, rim, clap | 44.1 kHz, mono, 16-bit **[inspected]** |
| B | **Boulevard 86, Screw, Terminal Mirage, MPC-ready 808** | 6× one-shot kits (same schema; not phonk-primary) | `main`: `disklordz/website/public/samples/{preset}/` | **Yes** — 6 WAV each | same 6 roles | 44.1 kHz mono 16-bit **[inspected]** |
| C | **Root phonk loop pack (5 files)** | Stereo loops | `main`: repo root `*phonk*.wav`, `90s-memphis-*.wav`, `hard-compressed-*.wav` | **Yes** — 5 WAV | 5 loops (no stems) | 44.1 or 48 kHz **stereo** 16-bit **[executed]** |
| D | **SparkLE 808 Lab sprint kits (5 sessions)** | One-shots + loops (+ MPC500 trees) | `main`: `docs/disklordz/spark-le/sprint-kits/session-*` | **Yes** | 8 one-shots + 1–3 loops each + MPC500 WAVs | 44.1 kHz mono 16-bit **[executed]** |
| E | **Lofi-12 phonk bank** (16-slot) | Full drum bank | **Branch only** PR [#69](https://github.com/samuelfreemanjobs-hash/Instruments/pull/69): `disklordz/lofi12-phonk-factory/` | **No** — generate locally | 16 slots (see §1.3) | **24 kHz mono 16-bit** default **[executed]** |
| F | **Lofi-12 phonk loops** | Rendered loops | Same PR #69: `phonk_loop_factory.py` | **No** | 1+ loops per run (`--batch N`) | **44.1 kHz mono 16-bit** (full); optional 12 kHz Lofi export **[executed]** |
| G | **Phonk Drum Studio → Disklordz kit** | 6× one-shot kit (VAE or folder) | **Branch only** PR [#88](https://github.com/samuelfreemanjobs-hash/Instruments/pull/88): `tools/phonk-drum-studio/` + `disklordz/sound-factory/kit_from_samples.py` | **No** | Same 6 SaaS slots as (A) | 44.1 kHz mono (kit normalizer) **[inspected]** |
| H | **Drum Loop Factory (Memphis mirror)** | External repo / tooling | PR [#87](https://github.com/samuelfreemanjobs-hash/Instruments/pull/87) (draft): publish scripts only | **No** in monorepo | Unknown until mirror populated | N/A **[inspected]** |

**Total distinct phonk-oriented sellable SKUs without running generators today:** **6 WAV files** in the only phonk-preset stub kit (Midnight Circuit) + **5 root loops** + **0** complete 16-slot banks (banks require PR #69 merge + generation).

SparkLE sprints are **committed** but only **session 2 (Screw)** and **session 4 (Trunk Sermon / Houston)** are adjacent to phonk/Memphis; they are **not** marketed as pure drift phonk on `main`.

---

### 1.2 Main-branch SaaS phonk stub kit (Midnight Circuit)

| Field | Detail |
|-------|--------|
| **Name** | Midnight Circuit (`midnight-circuit`) |
| **Type** | Procedural drum one-shot kit (v0 preview) |
| **Path** | `disklordz/website/public/samples/midnight-circuit/*.wav` + live regeneration in `disklordz/website/src/lib/generation/synth.ts` |
| **Contents** | 6 files: `kick`, `snare`, `hat_closed`, `hat_open`, `rim`, `clap` |
| **Format** | 44.1 kHz, mono, 16-bit PCM **[executed]** (measured via `wave` on `clap.wav`) |
| **Readiness** | **A few hours left** — audio exists and site can ZIP via API, but timbre is **synthetic sine/noise** (`generate_stub_kits.py` / `synth.ts`), not Memphis/phonk-grade. |
| **Licensing** | **Low risk** if labeled *100% synthesized*; avoid implying sampled TR-808 hardware. Preset copy mentions “808” / “Memphis” as **style words**, not trademarks. |
| **Ship steps (WooCommerce ZIP)** | 1. `python3 disklordz/sound-factory/scripts/generate_stub_kits.py` (refreshes stubs). 2. Build manifest: run site generate flow or `python3 disklordz/sound-factory/scripts/generate_kit.py --preset midnight-circuit --prompt "<product copy>" --out /tmp/kit`. 3. `cd /tmp/kit && zip -r ../disklordz-midnight-circuit-v1.zip manifest.json README.txt *.wav`. 4. Add `LICENSE.txt` (commercial synth disclaimer). 5. Upload to WooCommerce downloadable product. Optional: use `POST /api/download` with manifest for parity with SaaS **[inspected]**. |

Other four stub presets share the same machinery; only **midnight-circuit** is phonk-positioned in `presets.ts`.

---

### 1.3 Lofi-12 Phonk Drum Factory (primary factory — PR #69, draft)

| Field | Detail |
|-------|--------|
| **Name** | Lofi-12 Phonk Drum Factory |
| **Type** | Procedural phonk **16-slot banks**, **loops**, optional **vocal textures**, browser **sequencer** |
| **Path / PR** | Branch `cursor/lofi12-phonk-factory-f8f8` — [PR #69](https://github.com/samuelfreemanjobs-hash/Instruments/pull/69) (draft). **Not on `main`.** |
| **One-shot bank layout** | 16 slots per bank **[inspected]** `bank_layout.py`: 3 kicks (`kick_808`, `kick_dist`, `kick_sub`), Memphis snare + rim, clap, 3 hats, 2 cowbells, 2 toms, perc, 2 FX |
| **Output format (banks)** | **24 kHz mono 16-bit** WAV (`01_kick_808.wav` … `16_fx_riser.wav`) + `load_manifest.json` + copied `PATTERN_GUIDE.md` |
| **Output format (loops)** | **44.1 kHz mono 16-bit** loop WAV; manifest `loop_manifest.json` (`LOFI12_PHONK_LOOP_BATCH`) **[executed]** |
| **Committed audio?** | **No.** Only code, docs, GUI PNGs, tests. Banks/loops **must be regenerated**. |
| **Readiness** | **A few hours left** after merge — `python3 -m unittest discover -s disklordz/lofi12-phonk-factory/tests -q` → **26 OK** **[executed]**; `phonk_factory.py` generated 16 files in `/tmp/phonk_audit_bank` **[executed]**. Sequencer PR body cites manual UI pass — **not re-run here.** |
| **Licensing concerns** | Audio is **synthesized** (`phonk_synth.py`, drum machine **style profiles**: TR-808, TR-909, DR-660, SR-16, R-8 MKII, tape). Docs name **Juicy J / DJ Paul / DJ Toomp** as style presets — **do not use artist names on WooCommerce SKUs**; use “Memphis trinity style” / internal preset IDs. “SP-1200-style” is emulation language, not a ROM dump **[inspected]**. |
| **Ship steps — 16-slot bank ZIP** | 1. Merge PR #69. 2. `python3 disklordz/lofi12-phonk-factory/scripts/phonk_factory.py --prompt "drift phonk 140" --variation 0 --out ./dist/bank_v0` (repeat `--variation` or `--batch 4` for variants). 3. Human listen + pick keeper. 4. `zip -r diskordz-lofi12-phonk-bank-v1.zip dist/bank_v0/*.wav dist/bank_v0/load_manifest.json dist/bank_v0/PATTERN_GUIDE.md` + `README.txt` (24 kHz, Lofi-12 bank A, synthesized). 5. WooCommerce upload. |
| **Ship steps — loop pack ZIP** | 1. `python3 .../phonk_loop_factory.py --preset memphis_trinity --batch 8 --out ./dist/loops`. 2. QA BPM/feel. 3. Zip all `*.wav` + `loop_manifest.json` + `PHONK_PROGRAMMING_PLAYBOOK.md`. |
| **Ship steps — “Disklordz 6-slot” from SaaS** | `phonk_factory.py --from-disklordz <extracted SaaS zip>` merges purchased 6-slot kit into slots 1–6 mapping **[inspected]** `DISKLORDZ_MAP`. |

**Sequencer (same PR):** `disklordz/lofi12-phonk-factory/sequencer/` — renders loops via API (`serve.py`). Sellable as **companion tool**, not a WAV SKU by itself.

---

### 1.4 Phonk Drum Studio — VAE desktop (PR #88, draft)

| Field | Detail |
|-------|--------|
| **Name** | Phonk Drum Studio |
| **Type** | Desktop app: train VAE on **your** WAV folder, generate/mutate one-shots, export Disklordz kit |
| **Path / PR** | `tools/phonk-drum-studio/` on branch `cursor/phonk-drum-studio-2a8e` — [PR #88](https://github.com/samuelfreemanjobs-hash/Instruments/pull/88) (draft). CI: Windows PyInstaller artifact `PhonkDrumStudio-Windows-x64`. |
| **Kit packaging** | `disklordz/sound-factory/kit_from_samples.py` + `scripts/build_studio_kit.py` → `DISKLORDZ_DRUM_KIT_MANIFEST` with 6 slots, 44.1 kHz **[inspected]** |
| **Committed audio?** | **No.** No training corpus in repo. `.gitignore` on sound-factory outputs on branch **[inspected]**. |
| **Readiness** | **Needs real work** for retail kits — requires operator-provided **licensed** training folder, training time, listening QA, then `build_studio_kit.py --fill-stubs` for missing slots. App/tests exist; **no prebuilt sellable kit**. |
| **Licensing** | **High risk if misused** — VAE learns from user-supplied samples. WooCommerce SKU must state training data is **customer's responsibility** unless you ship a **Disklordz-owned** corpus. |
| **Ship steps** | 1. Merge PR #88. 2. Curate cleared one-shot folder. 3. Train → export WAVs. 4. `python3 disklordz/sound-factory/scripts/build_studio_kit.py --preset midnight-circuit --prompt "..." --samples-dir ./exports --out ./kit [--fill-stubs]`. 5. Zip like SaaS kit (§1.2). |

---

### 1.5 Drum Loop Factory (PR #87, draft)

| Field | Detail |
|-------|--------|
| **Name** | Drum Loop Factory (Memphis external repo mirror) |
| **Type** | Intended separate repo; monorepo PR adds **publish/bootstrap scripts only** |
| **Path / PR** | [PR #87](https://github.com/samuelfreemanjobs-hash/Instruments/pull/87) — `scripts/publish-drum-loop-factory-to-github.sh`, `scripts/cloud-agent-drum-loop-factory-install.sh` |
| **Kits in repo** | **0** — no `drum-loop-factory/` tree on `main` **[inspected]** |
| **Readiness** | **Needs real work** — secret `MEMPHIS_DRUM_LOOP_FACTORY_PUSH_TOKEN` + mirror population before any SKU. |
| **Ship steps** | N/A until external factory ships audio; then mirror ZIP workflow TBD. |

**Note:** Lofi-12 `phonk_loop_factory.py` (PR #69) is the **in-monorepo** loop generator today; do not confuse with PR #87's external factory.

---

### 1.6 Root-level phonk loop WAVs (committed on `main`)

| File | Duration (s) | Rate | Ch | Bit |
|------|-------------|------|-----|-----|
| `phonk-drums-doomshop-loop_160bpm.wav` | 12.0 | 44100 | 2 | 16 |
| `phonk-aggressive-drums-noisy-loop_140bpm.wav` | 13.7 | 44100 | 2 | 16 |
| `phonk-drum-loop-breakbeat_85bpm.wav` | 11.3 | 44100 | 2 | 16 |
| `90s-memphis-phonk-drum-loop-w-cowbell_140bpm.wav` | 6.9 | 44100 | 2 | 16 |
| `hard-compressed-memphis-phonk-drum-loop_130bpm_C#.wav` | 29.5 | 48000 | 2 | 16 |

**[executed]** measured via Python `wave` module.

| Field | Detail |
|-------|--------|
| **Readiness** | **Blocked for sale** until provenance review — git history shows `Add files via upload` (`b1b783c9`), not factory script output. |
| **Licensing** | **Unknown / high risk** — treat as **third-party or unverified** until documented source and clearance. Filename “memphis” / “phonk” does not prove originality. |
| **Ship steps** | 1. Trace origin with uploader. 2. If cleared, `zip -r diskordz-phonk-loops-preview.zip *.wav README LICENSE`. 3. If not cleared, **remove from repo** and regenerate via PR #69 factory instead. |

---

### 1.7 SparkLE sprint kits (phonk-adjacent committed packs)

All under `docs/disklordz/spark-le/sprint-kits/` on **`main`** (merged PR #95). Each session includes `OneShots/` (8 WAV), `Loops/` (1–3), `MPC500/**/*.WAV`, `spark-le-manifest.json`.

| Session | Preset lane | Phonk? | One-shots | Loops |
|---------|-------------|--------|-----------|-------|
| 1 Gangsta trap | `lane-trap-or-die` | Adjacent | 8 | 2 |
| 2 DJ Screw | `lane-dj-screw` | Slow / phonk-adjacent | 8 | 2 |
| 3 Smooth jazz rap | `lane-smooth-jazz-rap` | No | 8 | 1 |
| 4 Trunk Sermon | `lane-fat-pat` | Southern trunk (Houston) | 8 | 3 |
| 5 UK drill | `lane-uk-drill` | No | 8 | 2 |

**Format:** 44.1 kHz mono 16-bit one-shots **[executed]**.  
**Readiness:** **A few hours left** — committed audio + manifest; sprint reviews marked no “keepers” promoted yet **[inspected]** `SPRINT-PROGRESS.md`.  
**Licensing:** Generated via SparkLE export pipeline (procedural) — align copy with `808-LAB.md` (**archetypes, not artist samples**).  
**Ship steps:** 1. Pick session folder. 2. `zip -r diskordz-sparkle-session-2-screw.zip session-2-dj-screw/OneShots session-2-dj-screw/Loops session-2-dj-screw/spark-le-manifest.json`. 3. Add README + license. Optional: `npm run export:sprint-1-5` in `disklordz/website/` for refresh **[inspected]**.

**OG / drift phonk Spark lanes** (`lane-og-phonk`, `lane-drift-phonk`, etc.) exist in `lane-registry.ts` but **no dedicated sprint folder** with committed WAVs on `main`.

---

### 1.8 Phonk Box (not a sample pack)

`disklordz/phonk-box/` — Raspberry Pi / MIDI **engine** loading manifests; **no retail WAV SKU**. Relevant for hardware/MIDI buyers, not WooCommerce drum ZIPs **[inspected]**.

---

### 1.9 WooCommerce ZIP pattern (all phonk kits)

Common contract for **6-slot SaaS kits** (stubs, site generate, Phonk Studio export):

- `manifest.json` — `format: "DISKLORDZ_DRUM_KIT_MANIFEST"`, `presetId`, `samples[]` with `sha256`
- Six WAVs named `kick.wav` … `clap.wav`
- `README.txt` — prompt, preset, “44.1 kHz mono”
- Optional `LICENSE.txt`

Site builder reference: `disklordz/website/src/app/api/download/route.ts` **[inspected]**.

**Lofi-12 16-slot banks** use `load_manifest.json` / `LOFI12_PHONK_BANK` instead — not interchangeable with SaaS manifest without conversion script (manual mapping in `phonk_factory.py --from-disklordz` goes the other direction).

---

## 2. Other sellable products (brief)

| Name | Type | Location | Build / binary | Readiness | Licensing notes |
|------|------|----------|----------------|-----------|-----------------|
| **MeterTap** | FX / metering VST3 | `main`: `vst/MeterTap/`, `plugins/MeterTap/` | Linux build in CI pattern; **no** `release/MeterTap/*.zip` on `main` **[inspected]** | APC `ship_ready` in `status.json`; package step not committed | Own DSP — low sample risk |
| **WAVE-9090** | VST3 trap/phonk synth (sampleless) | `main`: `Wave9090/` | `build/Wave9090/.../WAVE-9090.vst3` present on VM **[executed]** | **A few hours** — needs signed installer, pluginval, PR [#102](https://github.com/samuelfreemanjobs-hash/Instruments/pull/102) Windows zip | No samples; “909” is style naming |
| **JD Upgraded** | VST3/CLAP synth | `main` `Source/` | Builds in CI | **Needs work** — Phase 5 / cleanroom ROM story | Roland JD branding — use “JD-style” carefully |
| **SP-1200 Drumulator** | Standalone + VST3 | `main` `SP1200/` | VST3 built on VM **[executed]** | **Needs work** — PR [#91](https://github.com/samuelfreemanjobs-hash/Instruments/pull/91) import QA; **Akai** trademark | User-provided samples; no Akai ROM in sell path |
| **NTS-1 bass osc library** | Logue user osc | `main` `src/oscillators/*`, PR [#110](https://github.com/samuelfreemanjobs-hash/Instruments/pull/110) mkII CI | `.prg` via `./tools/package-nts1.sh` (not run here) | **A few hours** after hardware smoke; names reference Juno/Moog/**808** styles | Korg platform; avoid “TR-808” product titles |
| **Plugg VA** | VSTi synth | Branch PR [#106](https://github.com/samuelfreemanjobs-hash/Instruments/pull/106) | Not on `main` | **Needs real work** | N/A |
| **Auto Sampler** | VST3 | Branch PR [#101](https://github.com/samuelfreemanjobs-hash/Instruments/pull/101) | Not on `main` | **Needs real work** | Depends on captured source material |
| **MPC one-shot pack** | 100 WAVs from Boom | Branch PR [#100](https://github.com/samuelfreemanjobs-hash/Instruments/pull/100) | Not on `main` | Unclear without branch checkout | Verify Boom/VST license |
| **Memphis vault batch WAV** | Procedural WAV gen | Branch PR [#104](https://github.com/samuelfreemanjobs-hash/Instruments/pull/104) | Not on `main` | **Needs work** | `[inspected]` title only |
| **Disklordz SaaS kits** | Digital download | `disklordz/website/` | Live product infra | **Shipped** for SaaS, not WooCommerce catalog sync | Stripe/Supabase path |

---

## 3. Ranked top 5 — closest to sellable (honest)

1. **Lofi-12 Phonk Drum Factory (PR #69)** — Complete generator, tests green, 16-slot + loops **[executed]**; **blocked on merge + human QA + SKU copy** (no committed masters). Best phonk **volume** play.
2. **SparkLE sprint kit — Session 2 (DJ Screw)** or **Session 4 (Trunk)** — **Committed WAVs** today; weak marketing as “phonk” but **fastest ZIP to WooCommerce** after copy + keeper sign-off.
3. **Midnight Circuit stub / live-generated 6-kit** — On `main`, parametric uniqueness via website; audio quality **preview-grade**, not boutique phonk.
4. **WAVE-9090 VST3** — Not a drum kit, but **phonk-adjacent** plugin with build artifacts; needs packaging/Windows PR #102 for wide OS.
5. **MeterTap VST3** — Factory reference **1.0.0** SKU; unrelated to phonk but **closest shipped plugin** on `main` (missing release zip in tree).

**Deliberately not on list:** root **5 loop WAVs** (provenance), **Phonk Drum Studio** retail kits (no corpus), **Drum Loop Factory** mirror (empty).

---

## 4. Recommended next actions (phonk-first)

1. **Merge PR #69** (or cherry-pick factory) and bake **3 fixed SKUs**: “16-shot Lofi bank”, “8 Memphis loops”, “Screw/phonk one-shot mini” from factory output.
2. **Quarantine or document** root `*phonk*.wav` loops before any shop listing.
3. Wire WooCommerce downloads to same ZIP layout as `POST /api/download` for parity.
4. Keep **artist names** off product pages; use lane IDs from `lane-registry.ts` / internal presets only.

---

## 5. Audit methodology

- Branch tips and PR metadata via `gh pr list/view` **[executed]**
- WAV inspection via Python `wave` **[executed]**
- Lofi factory tests + sample `phonk_factory.py` / `phonk_loop_factory.py` on branch files checked out transiently (not committed) **[executed]**
- Plugin paths inspected on `main`; full `run_business.py --profile ci` not run (out of scope for this doc-only PR)

*This file is the deliverable for the product inventory audit; update when PRs #69, #88, or #87 merge.*
