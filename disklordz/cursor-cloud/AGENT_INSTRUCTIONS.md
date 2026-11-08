# Cursor Cloud agent — V Voyager / Plugin Factory

**Agent ID (Airtable):** `cursor-v-voyager`

## Before coding

1. Read `/ARCHITECTURE.md` and `plugin-factory/ARCHITECTURE.md`.
2. For V Voyager: `plugin-factory/plugins/moog-voyager/README.md` and `PRESETS.md`.
3. Follow `AGENTS.md` QA: `python3 vst-testing-ops/run_business.py --profile factory` or `release`.

## Ship check

```bash
cd plugin-factory && ./scripts/factory.sh release
```

Expect `V Voyager.vst3` under `build/plugins/moog-voyager/.../VST3/`.

## Handoffs

- **Inbox:** `disklordz/cursor-cloud/inbox/CO-*.json` (Airtable → GitHub Action → commit).
- Pick up WO from JSON; branch `cursor/<short-description>-<suffix>` unless WO specifies otherwise.
- Do not merge or deploy production unless the user asks.

## Out of scope

- HISE / Antigravity lane (`antigravity-hise`) — use `disklordz/antigravity/inbox/` instead.
- Disklordz SaaS unless WO explicitly targets `disklordz/website/`.
