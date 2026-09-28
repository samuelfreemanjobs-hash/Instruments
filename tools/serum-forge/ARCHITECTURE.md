# Serum Forge — architecture

## Purpose

**Serum Forge** implements the **symbolic and guardrail layers** of an autonomous Serum preset factory: ParamSpec validation, archetype rules, DX7 rejection, intermediate JSON, and hooks for external **serum-preset-packager** / **Pedalboard** stages. It does not ship proprietary Serum binaries or a full CBOR encoder.

## Build & run

```bash
cd tools/serum-forge
pip install -r requirements.txt
pip install -e .
python scripts/smoke.py
python scripts/cli.py --prompt "warm analog pad" --archetype pad_ambient
uvicorn serum_forge.main:app --host 127.0.0.1 --port 8010
```

## Data flow

```text
prompt → semantic.intent_to_patch
      → guardrails.clamp_patch
      → compiler.write_intermediate_json
      → [optional] compiler.compile_serum_preset (SERUM_PACKAGER_BIN)
      → [optional] render.render_preset_headless (SERUM_VST3_PATH + pedalboard)
      → perceptual.* (CLAP / mel loss — stubs)
```

## Key modules

| Module | Role |
|--------|------|
| `main.py` | FastAPI (`/api/health`, `/api/pipeline/prompt`, `/api/schema`) |
| `models.py` | `SerumSymbolPatch` ParamSpec subset |
| `param_spec.py` | Pydantic → OpenAPI JSON schema for LLM constraints |
| `guardrails.py` | Headroom, reverb/resonance caps, archetype defaults |
| `dx7.py` | Reject DX7 SysEx ingestion |
| `compiler.py` | JSON intermediate + external packager subprocess |
| `pipeline.py` | Five-stage orchestration |
| `semantic.py` | Heuristic NL → patch (replace with LLM caller) |
| `render.py` | Pedalboard placeholder |
| `docs/` | Research index + Gemini Gem prompt |

## Threading / realtime

CLI and batch workers should use **process isolation** per Serum instance (plugin is not thread-safe). Scale with multiprocessing worker pools, not shared plugin handles.

## Extension points

- Wire `semantic.py` to Gemini/OpenAI with `param_openapi_schema()` tool output.  
- Add CMA-ES loop calling headless render + `perceptual` losses.  
- Integrate PySerum object model before packager CLI.  
- Do **not** add DX7 SysEx importers to this package.

## Related docs

- [docs/SERUM_TEXT_TO_PRESET_PROMPTS.md](docs/SERUM_TEXT_TO_PRESET_PROMPTS.md) — Trap/phonk Serum prompt library  
- [SynthForge](../synth-forge/ARCHITECTURE.md) — hardware preset workstation  
- Root [ARCHITECTURE.md](../../ARCHITECTURE.md)
