# Loop SOP — Hardware Preset Designer

## Primary verify commands

```bash
cd tools/synth-forge
pip install -r requirements.txt && pip install -e .
python3 -m pytest tests -v
python3 ../../disklordz/integrations/agents/hardware_preset_designer.py --self-test
python3 ../../disklordz/integrations/agents/hardware_preset_designer.py \
  --prompt "Cardo luxury cruising pad" --synth-id minilogue_xd --count 4
```

Optional studio smoke:

```bash
uvicorn synth_forge.main:app --host 127.0.0.1 --port 8000
curl -s http://127.0.0.1:8000/api/health
```

## Control loop

```text
READ tools/synth-forge/ARCHITECTURE.md + user brief
→ choose synth_id + category or prompt
→ generate (generator / semantic_engine / cloner)
→ clamp_for_hardware on every export path
→ verify (pytest | self-test | preview metrics)
→ report JSON + file paths; disclose stub adapters
```

## Terminals

`completed` | `unsafe_params_rejected` | `adapter_stub_only` | `tests_failed`

## Invariants

- No SysEx/binary export without `clamp_for_hardware`.
- Every deliverable cites [executed] command output.
- MiniFreak/Zenology exports must say **stub** in the user-facing summary.
