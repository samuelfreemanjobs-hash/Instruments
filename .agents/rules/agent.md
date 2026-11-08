# APC agent rules (Instruments)

1. Read `AGENTS.md` and `plugins/<Name>/status.json` before plugin work.
2. Use `scripts/lib/apc-paths.sh` — never hardcode `plugins/` if config overrides.
3. Dream/Plan/Design: no production C++ in `plugins/` only markdown + json.
4. Implement: code in `vst/<Name>/` via `tools/new-vst.sh`.
5. One phase per turn unless `/apc-new`.
6. Follow `docs/dsp-architecture-specification.md` for program/surface/global/mapping.
7. No AI co-author git trailers.
