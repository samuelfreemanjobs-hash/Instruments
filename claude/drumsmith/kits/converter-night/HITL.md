# Converter Night — HITL checklist

**`release_ready` must stay false until every required box is human-checked.**

## Required (RELEASE_GATE)

- [x] W2 originals rendered (synthesis — no ref dumps in ship tree) — `W2_RENDER.md`
- [x] W3 `verify_kit.py` technical PASS + diversity — local `qa-report.json` / `diversity-report.json`
- [x] CALIPER read-only audit draft — `CALIPER_NOTES.md` (verdict BLOCKED/UNVERIFIED for ship)
- [x] Listening log: reviewer + YYYY-MM-DD + path — Sam Freeman · 2026-11-18 · decay re-listen (`listen/LISTENING_LOG.md`, `converter-night-listen`)
- [ ] Release approval: owner + YYYY-MM-DD
- [ ] Provenance + license + store copy reviewed
- [ ] No trademarked machine product title in storefront
- [ ] Device claims only where TESTED

## Commercial

- [ ] Price lock (after comps)
- [ ] Cover / preview demos
- [ ] EULA final (promote `_docs_draft` → packaged `_DOCS`)

## Optional RE upgrade

- [ ] **Re-attach Boss DR-660 zip** (not on this agent — only DR-5/DR-55 present) → stage `references/dr660/` → re-meter RATE; proxies = tr808 808_CHARACTER until then · see `docs/IMPROVEMENTS.md`
- [ ] Upload SP-1200 / Drumulator → re-meter BIT12

## Agent cannot

Self-authorize listening or release. Do not set `release_ready=true` without Sam.
