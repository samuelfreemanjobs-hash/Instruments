# CALIPER — Cursor invocation

CALIPER is a **read-only** skeptical QA auditor. Policy: `.claude/agents/CALIPER.md`.

## When to run

After W3 (`verify_kit.py` / diversity) and before any W4 “ready” language. Also when a human challenges a technical PASS.

## Cursor procedure

1. Ensure reports exist (do not invent meters):

```bash
cd claude/drumsmith
python3 scripts/verify_kit.py --kit <KIT> --manifest <KIT>/manifest.json --json <KIT>/qa-report.json
python3 scripts/diversity_audit.py --kit <KIT> --json <KIT>/diversity-report.json
```

2. Open a **Task** subagent (or a fresh chat) with this prompt:

```text
You are CALIPER. Read claude/drumsmith/.claude/agents/CALIPER.md.
Audit kit <KIT>: manifest.json, qa-report.json, diversity-report.json, provenance docs.
Do not modify files. Bash only for read-only inspection.
Report PASS | FAIL | BLOCKED/UNVERIFIED with evidence paths.
Separate observed vs assumed. Flag sample-peak-as-true-peak and weak pitch claims.
release_ready stays false unless human listening + release approvals exist.
```

3. Paste CALIPER’s verdict into the kit’s `docs/` or the W5 notes. Do **not** treat the main DRUMSMITH agent’s self-check as a CALIPER PASS.

## Anti-patterns

- Editing hashes / manifests to clear FAIL  
- Claiming FORGE JSON means a VST exists  
- Marking UNVERIFIED devices as TESTED  
- Approving release from technical PASS alone  
