# DRUMSMITH v2.0 — Native Synthesis Edition (agent build)

**Monorepo:** `claude/drumsmith/` · Status: `project.md` · Workbench: `workbench/DRUMSMITH_Workbench.html`  
**Phase:** agent package + native renderer CI — **not** a commercial kit release.

```bash
cd claude/drumsmith
python3 scripts/self_test.py && python3 scripts/test_upgrade.py && python3 scripts/test_v13.py
CXX=g++-12 python3 scripts/test_v20.py
cmake -S native -B native/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-12
cmake --build native/build -j && ctest --test-dir native/build --output-on-failure
python3 scripts/native_factory.py --out /tmp/Native_TLS --renderer native/build/drumsmith-render \
  --genre drift-phonk --count 64 --sku TLS --plan tail-light-static
```

A Claude Code repo-local 4-layer agent configuration with one read-only CALIPER QA subagent, three skills, portable Python 3 WAV tools, and a native C++20 renderer. No API key is needed for scripts; using Claude Code requires access to a compatible model/service and is **not** necessarily free. Scripts use Python stdlib; optional `ffmpeg` enables R128 checks.

## Install
1. In this monorepo, use `claude/drumsmith/` as the package root (repo-root `.claude` symlinks agents/skills).
2. Open a terminal in that directory. Run `python3 scripts/self_test.py` (or `py -3 scripts/self_test.py` on Windows).
3. Optionally install ffmpeg and add it to PATH. Confirm `ffmpeg -version`.
4. In Claude Code started from repository root, ask **“DRUMSMITH: W1 SPEC for Tail Light Static using INTAKE.md. Do not imply files have been rendered.”** Agent/project instructions will auto-load in compatible Claude Code setups. To invoke CALIPER, ask it to audit a pre-generated report, or delegate to CALIPER using the Agent tool if available.

## CLI demonstration (technical pipeline, not a completed product)
```sh
python scripts/scaffold_kit.py --out demo_kit --sku TLS --count 3
python scripts/render_reference.py --kit demo_kit
python scripts/verify_kit.py --kit demo_kit --manifest demo_kit/manifest.json --json demo_kit/qa-report.json
python scripts/verify_kit.py --kit demo_kit --manifest demo_kit/manifest.json --ffmpeg
```
`render_reference.py` generates original *experimental reference sounds* only, not a mastered commercial pack. `verify_kit.py` tests WAV technical quality; it cannot listen. Nonzero exit means at least one hard technical failure. The manifest uses SHA-256 after rendering; edit/re-render files only with corresponding manifest updates.

## Architecture Decision Record (ADR-001)
- Single DRUMSMITH orchestrator avoids coordination overhead and owns product + engineering decisions.
- CALIPER read-only reviewer challenges claims; it cannot approve releases.
- JSON manifest is the machine-readable source of truth; YAML configuration is human-readable policy. Portable stdlib tools minimize setup and vendor lock-in.
- LUFS and true peak are delegated to ffmpeg (optional), not approximated by WAV sample peaks.
- Device profiles start UNVERIFIED, even when generic PCM format is valid.
- Production JUCE 8/C++20 work is explicitly handed to FORGE, not silently embedded here.

## Measured vs asserted
`verify_kit.py` sample-peak and RMS values are exact within PCM quantization. Its clipped fraction counts PCM full-scale occurrences and is **not** a true-peak measurement. Optional ffmpeg values use the installed build's EBU R128 meter and are reported with metadata. Tune and listening decisions must be corroborated by humans.

## Folder contents
`CLAUDE.md`, `SOUL.md`, `DUTIES.md`, `SYSTEM_PROMPT.md`, `agent.yaml`, `INTAKE.md`, `.claude/agents/CALIPER.md`, `.claude/skills/*`, `config/core-config.yaml`, `scripts/`, `docs/provenance/`, `examples/tail-light-static/`.

## v1.1 improvements (November 17, 2026)
- A real 64-hit Tail Light Static layout matching the draft intake counts: 8 808 / 8 kick / 8 snare / 6 clap / 12 hat / 8 cowbell / 8 perc / 6 FX.
- A new `scripts/pitch_check.py` evaluates sustained pitched 808 tails by autocorrelation AND a target-centered spectral search (stdlib, not an FFT/YIN implementation). Pitch targets are validated by default; inconclusive checks are warned, not silently passed as tuned.
- Human release/listening approvals now require reviewer/date/evidence metadata. Explicit `required_device_profiles` block readiness unless marked TESTED. TESTED is a declaration, not a live hardware test.
- Render preflight blocks traversal paths and existing target files before first output is written. Added distinct experimental clap and FX recipes.
- 28 deterministic tests, including pitch mismatch, release evidence, device-gate and path-security regressions.

### Full draft kit command sequence
```powershell
python scripts/scaffold_kit.py --out TLS_64 --sku TLS --plan tail-light-static
python scripts/render_reference.py --kit TLS_64
python scripts/verify_kit.py --kit TLS_64 --manifest TLS_64/manifest.json --json TLS_64/qa-report.json
```
The generated WAVs are **experimental placeholders**, not 64 distinct, commercial-ready voices; variation/production sound design and human listening remain required. For auditing and approvals see `docs/RELEASE_GATE.md`.

## v1.2 — Recipe Production Upgrade (2026-11-17)
**New working commands** (Python stdlib, no API key):
```powershell
python scripts/self_test.py
python scripts/test_upgrade.py
python scripts/build_recipe_kit.py --out TLS_RECIPE_64 --sku TLS --plan tail-light-static
python scripts/render_recipes.py --kit TLS_RECIPE_64
python scripts/verify_kit.py --kit TLS_RECIPE_64 --manifest TLS_RECIPE_64/manifest.json --json TLS_RECIPE_64/qa-report.json
python scripts/diversity_audit.py --kit TLS_RECIPE_64 --json TLS_RECIPE_64/diversity-report.json
python scripts/export_forge.py --kit TLS_RECIPE_64 --out TLS_RECIPE_64/forge-handoff.json
```
New modules: `recipe_engine.py`, `build_recipe_kit.py`, `render_recipes.py`, `diversity_audit.py`, `export_forge.py`, `test_upgrade.py`. The old v1.1 reference tools continue to work. Recipe manifests are schema v1 for backward compatibility but use `parametric-v2` entries with hash-pinned DSP parameters. Every rendered WAV has an independent SHA-256. Modified recipes are rejected before rendering and flagged by QA. Drift phonk kit layouts are *reference experiments*, not commercial sound-design claims.

**Distinction**: This is a deterministic offline Python synthesis implementation, **not** a compiled JUCE/VST3 plugin and not a production-ready DSP engine. FORGE handoff JSON is a parameter contract awaiting C++ implementation and audio/real-time validation. All machine profiles remain UNVERIFIED. Noise, clipping, and pitch are still subject to listening. Simple within-family RMS-envelope comparisons are only advisory, not proof of perceptual uniqueness. Do not use the reference sounds commercially without listening/curation and release gates.

---

## v1.3 — Genre Factory + Local Audition Browser

**New:** Five original genre-guided recipes (`drift-phonk`, `memphis-tape`, `detroit-trap`, `southern-trap`, `cloud-rap`), reproducible batch orchestration, local read-only browser, six additional tests, and a separate release-blocking technical QA report. Genre profiles are creative starting points, not faithful recreations of any commercial machine or artist. The browser binds to `127.0.0.1` and needs no cloud login.

```powershell
# From the DRUMSMITH repo root
python scripts/factory.py --out my_phonk_kit --genre drift-phonk --sku DKL --count 64
python scripts/kit_browser.py --kit my_phonk_kit --port 8765
# Open http://127.0.0.1:8765 in your Windows browser; Ctrl+C to stop.
```

For a Tail Light Static category distribution:

```powershell
python scripts/factory.py --out tls_new --genre drift-phonk --sku TLS --plan tail-light-static
```

Choose another name for each run. **The factory refuses existing output directories**. Generated outputs include `manifest.json`, `masters/*.wav`, `qa-report.json`, `diversity-report.json`, and `forge-handoff.json`. Only `technical_qa: PASS` means numeric QA passed; **it does not authorize sale**. Listen and record approval, complete provenance, and verify each hardware compatibility claim separately. No physical-device or VST3 builds were tested.

Test all layers:

```powershell
python scripts/self_test.py
python scripts/test_upgrade.py
python scripts/test_v13.py
```

Details: `docs/WORKFLOW_V13.md`.


## v2.0 native synthesis implementation
See [README_V2.md](README_V2.md) and [docs/V2_NATIVE.md](docs/V2_NATIVE.md) for the C++20 renderer, tests, and JUCE VST3/standalone source build. JUCE build is not verified here.
