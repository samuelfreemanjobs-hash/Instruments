# DRUMSMITH Workbench (v1 template)

Single-file UI: kit spec, voice recipes, DSP calculators, in-browser WAV verify, docked DRUMSMITH agent (Claude **ask** + page tools when hosted on a platform that exposes those capabilities).

## Local open (offline tabs only)

Agent chat requires the hosted artifact runtime. Offline you still get calculators, spec builder, previews, and verify.

```bash
cd claude/drumsmith/workbench
python3 -m http.server 8767 --bind 127.0.0.1
# http://127.0.0.1:8767/DRUMSMITH_Workbench.html
```

## Parity contract

In-page `DM.*` and delivery checks are ported from:

- `../scripts/dsp_math.py`
- `../scripts/verify_kit.py`

Canonical regression: `python3 ../scripts/self_test.py` (19 tests). JS↔Python cross-check on 8 fixture WAVs documented in `../project.md`.

## Published copy

Private hosted page (full agent): see `../project.md` → Artifact links.

## Files

| File | Role |
|------|------|
| `DRUMSMITH_Workbench.html` | Template / source of truth in git |