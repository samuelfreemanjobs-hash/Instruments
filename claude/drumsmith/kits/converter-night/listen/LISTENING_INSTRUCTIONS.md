# Converter Night — HITL checklist

**`release_ready` must stay false until every required box is human-checked.**

## Required (RELEASE_GATE)

- [x] W2 originals rendered (synthesis — no ref dumps in ship tree) — `W2_RENDER.md`
- [x] W3 `verify_kit.py` technical PASS + diversity — local `qa-report.json` / `diversity-report.json`
- [x] CALIPER read-only audit draft — `CALIPER_NOTES.md` (verdict BLOCKED/UNVERIFIED for ship)
- [ ] Listening log: reviewer + YYYY-MM-DD + path
- [ ] Release approval: owner + YYYY-MM-DD
- [ ] Provenance + license + store copy reviewed
- [ ] No trademarked machine product title in storefront
- [ ] Device claims only where TESTED

## Commercial

- [ ] Price lock (after comps)
- [ ] Cover / preview demos
- [ ] EULA final (promote `_docs_draft` → packaged `_DOCS`)

## Optional RE upgrade

- [ ] Upload Boss DR-660 WAV archive (missing from prior uploads) → re-meter RATE; proxies = tr808 808_CHARACTER
- [ ] Upload SP-1200 / Drumulator → re-meter BIT12

## Agent cannot

Self-authorize listening or release. Do not set `release_ready=true` without Sam.
