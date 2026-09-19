# Sound Designer Agent contract

AI **generates patch instructions (JSON)**, not arbitrary DSP code or mystery audio.

## Input (example)

```yaml
sourceMultisample: DL_MULTI_000004   # e.g. piano
targetName: Dark Cinema Piano
character: [dark, warm, nostalgic, lo_fi]
genre: [phonk, wave_phonk]
macros: [character, brightness, drive, space, movement, width, decay, crush]
```

## Output (must validate against PATCH_FORMAT.md)

```json
{
  "patch": {
    "id": "DL_PATCH_000900",
    "name": "Dark Cinema Piano",
    "tones": [
      { "multisampleId": "DL_MULTI_000004", "romBank": 0, "program": 0, "level": 1.0 },
      { "multisampleId": "DL_MULTI_000881", "romBank": 9, "program": 2, "level": 0.12 }
    ],
    "macros": { "character": 0.7, "brightness": 0.35, "crush": 0.4 }
  },
  "taxonomy": {
    "instrumentType": "PIANO",
    "mood": ["dark", "cinematic"],
    "genre": ["phonk"]
  }
}
```

## Rules

1. Reference **IDs only** — never bare filenames.
2. Tones must resolve to existing multisamples in mounted content.
3. Macro values ∈ [0, 1].
4. Engine applies JSON; agent does not edit C++.

## Naming formulas

`[Descriptor] + [Instrument]` — e.g. Midnight Bell, VHS Keys, Detroit Sub.  
Maintain preset catalog to prevent duplicate display names.

## QC

All agent output runs through `ContentValidation` before merge to factory catalog.
