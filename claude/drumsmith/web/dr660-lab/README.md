# DR-660 Memphis DSP Lab

Browser lab for Memphis-style **16-step** sequencing (Web Audio lookahead + swing) plus Memphis FX bus: cassette WaveShaper, 12-bit bitcrush, lo-fi tape delay.

```bash
cd claude/drumsmith/web/dr660-lab
python3 -m http.server 8770 --bind 127.0.0.1
# http://127.0.0.1:8770/
node --test tests/*.test.mjs
```

See `ARCHITECTURE.md`. Continuity: Gemini handoff → sequencer → FX bus → bitcrush + tape delay.

**Lock + Mutate:** lock kick/snare (default), then **MUTATE UNLOCKED** for new hat/perc/cowbell variations.

Factory OS monolith + Claude/Antigravity plan: [`../memphis-loop-factory/`](../memphis-loop-factory/).
