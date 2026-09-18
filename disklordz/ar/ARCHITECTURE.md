# A&R & Artist agents — architecture

```text
Reference sessions (OpenClaw / Isaac / hardware)
        │
        ▼
  ar-reference-curator → brief JSON / session note
        │
        ▼
  Factory (POST /api/generate, batch) → candidate WAVs
        │
        ▼
  artist-midnight-circuit (lane) + ar-kit-qc
        │
        ▼
  CROW sign-off → PM marks WO Done → HELM launch / FORGE queue
        │
        ▼
  VARA sells (copy already matched to lane tags)
```

## Data

| Asset | Location |
|-------|----------|
| Lane presets | `website/src/lib/presets.ts` |
| RAG snippets | `website/src/lib/rag/knowledge-snippets.ts` |
| Session notes (text) | `disklordz/ar/sessions/*.md` |
| QC checklist | `disklordz/ar/QC_TRIPWIRE_SAMPLE.md` |

## Extension

- Add `artist-*` when new YouTube lane ships
- FORGE registers on push under `disklordz/ar/`
