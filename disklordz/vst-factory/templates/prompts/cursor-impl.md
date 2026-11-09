DiskLordz VST Factory — implementer prompt

Plugin: **{{plugin_name}}**
Class: {{plugin_class}}
Track: APC greenfield (`vst/{{plugin_name}}/`)

## Operator prompt

{{prompt}}

## Bound spec

```json
{{spec_json}}
```

## Suggested modules (reuse before inventing)

DSP: {{dsp_modules}}
GUI: {{gui_modules}}

## Read first

- `docs/dsp-architecture-specification.md`
- `plugins/{{plugin_name}}/status.json`
- `vst/{{plugin_name}}/README.md`

## Acceptance

- Compiles: `./tools/build-vst.sh {{plugin_name}}`
- Tests: `python3 vst-testing-ops/run_business.py --profile factory --plugin {{plugin_name}}`
- Report limitations + next slice
