# project.md — DRUMSMITH (Drum Sample & Kit Product Designer + DSP Audio Engineer Agent)

**Updated:** 2026-11-17 · **Phase:** **agent build** (not a commercial kit release) · **In-repo:** **v1.3** · Workbench HTML · Tail Light Static as the first exercise kit

## Scope

Four-layer agent + CALIPER for Disklordz: design kits, synthesize reference sounds, run technical QA, hand specs to FORGE. **Current goal = ship a working agent package in the monorepo**, with CI-green scripts and a Tail Light Static example path. Storefront / commercial masters / device-certified packs are **later** — release-gate machinery stays in-tree so we do not invent a fake “ready” claim, but it is not the active milestone.

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

### Later (not this phase)

`docs/RELEASE_GATE.md` exists for when Sam wants a commercial pack. Until then: technical PASS on examples is enough for agent development; do not chase ear-signoff or device TESTED flags for this PR.

## To do (agent build)

- [ ] Exercise W1–W5 end-to-end from Claude Code / workbench on Tail Light Static (agent UX)
- [ ] Keep CI green on script changes (`self_test` + `test_upgrade` + `test_v13`)
- [ ] Optional: tighten agent docs so “technical PASS” never reads as storefront-ready
- [ ] Optional DPCS on factory/render scripts
- [ ] Deferred: ear review, release authorization, device profiles, ffmpeg R128, v2.0 JUCE factory

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
