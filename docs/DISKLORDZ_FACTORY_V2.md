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

## v2.1 engine pass (same provenance)

- **Kick:** exponential pitch glide, sub harmonic, filtered click transient
- **Snare:** fixed HPF state, dual-tone body + bandpassed snap
- **Hats:** metallic partial stack + HP noise (open/closed)
- **Loops:** velocity humanization, creative swing, wildness-driven trap rolls + open hat
- **Master:** light parallel punch before RMS normalize

## Continuous improvement (WO-SAAS-018)

Daily scheduled regression and optional Cursor Cloud Agent: [DISKLORDZ_FACTORY_DAILY_AGENT.md](DISKLORDZ_FACTORY_DAILY_AGENT.md). Local check: `npm run factory:dsp-regression`.

## Not in scope (future WOs)

- Async Stable Audio / GPU worker (`disklordz/sound-factory/`)
- JUCE offline render of Wave909 DSP
- Retrieval over a licensed sample catalog (Splice-scale)
