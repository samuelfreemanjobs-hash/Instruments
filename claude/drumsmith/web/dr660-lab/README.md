# DR-660 Memphis DSP Lab

Browser lab for Memphis-style **16-step** sequencing (Web Audio lookahead + swing) with cassette/filter stubs.

```bash
cd claude/drumsmith/web/dr660-lab
python3 -m http.server 8770 --bind 127.0.0.1
# http://127.0.0.1:8770/
node --test tests/sequencer_math.test.mjs
```

See `ARCHITECTURE.md`. Continuity: Gemini handoff → sequencer first; cassette curves + filter next.
