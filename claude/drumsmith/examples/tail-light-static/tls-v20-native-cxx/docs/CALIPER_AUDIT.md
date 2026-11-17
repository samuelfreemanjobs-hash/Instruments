# CALIPER audit — tls-v20-native-cxx

**Auditor role:** CALIPER (read-only) · **Date:** 2026-11-17  
**Evidence:** `manifest.json`, `qa-report.json`, `diversity-report.json`

## Verdict: BLOCKED (commercial) / PASS (technical agent-demo)

| Gate | Result | Evidence |
|------|--------|----------|
| Deterministic verifier | PASS | `qa-report.json` technical_status PASS, errors [] |
| Exact duplicates | PASS | diversity exact_duplicate_groups [] |
| Sample-peak vs true-peak | OK framing | qa_scope states technical PCM only; no ITU claim observed in report |
| Pitch screens (808) | PASS screens | e.g. DKL-001 pitch_check PASS within 35¢ — not certified tuner |
| Human listening | FAIL / missing | human_listening_approved false; warning HUMAN GATE |
| Release approval | FAIL / missing | release_approved false |
| Device profiles | UNVERIFIED | hardware_targets UNVERIFIED |
| FORGE/plugin claim | N/A | no plugin binary claimed in manifest status NATIVE_RENDERED_NOT_RELEASED |
| Advisory similarity | REVIEW | similar_envelope_pairs_advisory non-empty — not a hard FAIL |

**Release recommendation:** Do not ship commercially. Agent-demo / CI reference use OK with `release_ready=false`.

## Remediation

1. Sam listening notes → `docs/listening-notes.md`  
2. Resolve advisory pairs by ear or recipe diversity  
3. Device import tests before any TESTED label  
4. Explicit release approval artifact before W4  
