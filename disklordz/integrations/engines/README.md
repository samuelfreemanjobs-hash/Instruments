# Optional audio engine workers

Disklordz website defaults to **parametric** synthesis. Set `DISKLORDZ_ENGINE=remote` and point URLs at these workers.

| Upstream repo | Folder | Env var |
|---------------|--------|---------|
| facebookresearch/audiocraft | [audiocraft/](audiocraft/) | `DISKLORDZ_AUDIOCRAFT_ENGINE_URL` |
| Stability-AI/stable-audio-tools | [stable-audio/](stable-audio/) | `DISKLORDZ_STABLE_AUDIO_ENGINE_URL` |
| replicate/cog | [cog/](cog/) | Deploy Cog container; use its HTTP URL |
| modal-labs/modal-client | [modal/](modal/) | Modal web endpoint URL |

## trigger.dev

For jobs longer than Inngest on Vercel, mirror `website/src/inngest/functions.ts` in a Trigger.dev project (`@trigger.dev/sdk`). Not bundled in the website build.

## Contract

Workers accept POST JSON:

```json
{ "sample": "kick", "prompt": "...", "spec": { }, "seed": 1, "provider": "audiocraft" }
```

Response:

```json
{ "pcm": [0.0, 0.01], "sampleRate": 44100 }
```

Fallback: parametric render in `engine-render.ts`.
