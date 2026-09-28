# Serum Forge

Autonomous **Serum** preset pipeline: constrained symbolic ParamSpec, guardrails, optional binary packager hook, headless render stubs.

```bash
cd tools/serum-forge
pip install -r requirements.txt
python scripts/smoke.py
python scripts/cli.py --prompt "dusty lofi ambient pad" --out-dir ./out
python scripts/cli.py --schema   # OpenAPI JSON for LLM tools
uvicorn serum_forge.main:app --port 8010
```

Set `SERUM_PACKAGER_BIN` and `SERUM_VST3_PATH` per [.env.example](.env.example) for stages 3–4.

Docs: [docs/SERUM_BINARY_ARCHITECTURE.md](docs/SERUM_BINARY_ARCHITECTURE.md) · [docs/GEM_SERUM_FORGE.md](docs/GEM_SERUM_FORGE.md)

See [ARCHITECTURE.md](ARCHITECTURE.md). Distinct from [SynthForge](../synth-forge/ARCHITECTURE.md) (hardware SysEx adapters).
