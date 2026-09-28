# Memphis Phonk Architect

Single-file **MEMPHIS-660** kick + **MEMPHIS-DR660** snare WAV studio (Tailwind UI, SVF filters, ADSR). Gemini Gem copy-paste instructions live under [docs/](docs/README.md).

```bash
cd disklordz/memphis-architect && python3 -m http.server 8765
python3 scripts/smoke.py
python3 scripts/generate_kick.py && python3 scripts/generate_snare.py
```

See [ARCHITECTURE.md](ARCHITECTURE.md).
