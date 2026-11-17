# project.md — DRUMSMITH (Drum Sample & Kit Product Designer + DSP Audio Engineer Agent)

**Updated:** 2026-11-17 · **In-repo:** **v1.3** (merged from `drumsmith-v1.3.zip`; v1.2 zip retained as lineage) · Workbench HTML · Tail Light Static W1 notes · **Commercial release still blocked**

## Scope

Four-layer agent + CALIPER: kit product design and offline DSP reference synthesis for Disklordz / Tail Light Static / SEANCE BOX; FORGE gets JSON handoffs (not a compiled VST3). Genre factory (v1.3), recipe engine (v1.2), verify + diversity + kit browser.

**Monorepo path:** `claude/drumsmith/`

## Done

- [x] v1.0 agent package + workbench template landed earlier on this PR
- [x] **Merge drumsmith-v1.3.zip** into `claude/drumsmith/` (supersedes v1.0/v1.1 scripts; includes v1.2 recipe stack)
- [x] Automated tests on Cloud: `self_test` 28 OK · `test_upgrade` 10 OK · `test_v13` 6 OK (**44/44**)
- [x] `verify_kit.py` on `examples/tail-light-static/tls-v13-drift` — **64/64**, exit 0, `technical_status=PASS`, `release_ready=false` (human gates + devices unverified)
- [x] CI workflow runs all three test suites + TLS drift verify
- [x] Preserve `workbench/`, `kits/tail-light-static/` intake/spec, monorepo `ARCHITECTURE.md`

## Announced lineage (packages)

| Version | Status in repo |
|---------|----------------|
| v1.0 tarball | Superseded by v1.3 tree |
| v1.2 zip | Uploaded; capabilities covered by v1.3 merge |
| **v1.3 zip** | **Canonical tree** |

### v1.3 surface

- Genres: drift-phonk, memphis-tape, detroit-trap, southern-trap, cloud-rap (`genre_profiles.py`)
- `factory.py` → render → `verify_kit.py` → `diversity_audit.py` → `export_forge.py`
- `kit_browser.py` @ `http://127.0.0.1:8765`
- Example kits under `examples/tail-light-static/` (`tls-64-recipes`, `tls-v13-drift`, small `reference-demo`)

### Ship gates

Technical PASS ≠ commercial release. Ear approval + device profiles still required. Reference WAVs are experimental (often 16-bit mono @ 44.1 kHz in examples). Pitch checks are dependency-free approximations.

## To do

- [ ] Sam: open workbench / kit_browser; ear-review TLS drift kit (and v1.2 envelope-similar pairs if still relevant)
- [ ] Named release authorization when ready (`docs/RELEASE_GATE.md`)
- [ ] Confirm device profiles; flip UNVERIFIED
- [ ] Optional ffmpeg R128 / certified true-peak for release claims
- [ ] Optional DPCS on factory/render scripts
- [ ] v2.0 roadmap: JUCE 8 drum manufacturing system (FORGE lane)

## Quick verify

```bash
cd claude/drumsmith
python3 scripts/self_test.py && python3 scripts/test_upgrade.py && python3 scripts/test_v13.py
python3 scripts/verify_kit.py --kit examples/tail-light-static/tls-v13-drift \
  --manifest examples/tail-light-static/tls-v13-drift/manifest.json
```

## Artifact links

- Workbench (hosted): https://claude.ai/artifact/FwE8VaPi2a5vXRMPoX19zH
- Workbench (git): `workbench/DRUMSMITH_Workbench.html`
