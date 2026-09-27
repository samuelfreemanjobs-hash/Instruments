# Disklordz Factory v2 (WO-SAAS-017)

Procedural drum generation upgrade for the SaaS **Drum Factory** path (no external ML worker in this tier).

## What changed

| Layer | v1 | v2 |
|-------|----|----|
| Synthesis | Simple sine/noise hits | 808-style kick, filtered snare/hats, richer envelopes |
| Master bus | Per-sample peak normalize | RMS target, HPF, optional bitcrush/tape (creative), quality gate |
| Stereo | Mono only | Optional width from `GenerationSpec.stereo` → interleaved stereo WAV |
| Provenance | `factory_*_v1` | `factory_studio_v2` / `factory_creative_v2` |

## Code map

- `disklordz/website/src/lib/generation/dsp-core.ts` — PRNG, filters, soft clip, bitcrush
- `post-process.ts` — `masterSample()` master chain
- `quality-gate.ts` — peak/silence/clip checks + `ensureAudible`
- `engine-render.ts` — `gritFromParams()`, raw render + provenance
- `factory.ts` — all kit/loop/sfx/pack writes go through `finalizeForWav()` before `encodeWav`

## Testing locally

```bash
cd disklordz/website && npm ci && npm run build
npm run start &
DISKLORDZ_URL=http://127.0.0.1:3000 npm run verify:go-live
```

## Not in scope (future WOs)

- Async Stable Audio / GPU worker (`disklordz/sound-factory/`)
- JUCE offline render of Wave909 DSP
- Retrieval over a licensed sample catalog (Splice-scale)
