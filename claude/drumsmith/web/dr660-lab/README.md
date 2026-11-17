# DR-660 Memphis DSP Lab

Browser lab for Memphis-style **16-step** sequencing (Web Audio lookahead + swing) plus Memphis FX bus: cassette WaveShaper, 12-bit bitcrush, lo-fi tape delay.

```bash
cd claude/drumsmith/web/dr660-lab
python3 -m http.server 8770 --bind 127.0.0.1
# http://127.0.0.1:8770/
node --test tests/*.test.mjs
```

See `ARCHITECTURE.md`. Continuity: Gemini handoff → sequencer → FX bus → bitcrush + tape delay.
