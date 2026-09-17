# TR-808 PWA (Disklordz)

## Purpose

Single-file **Progressive Web App**: playable **Roland TR-808**–style virtual analog drum machine in the browser. Lead-magnet / SaaS front-end; pairs with [DISKLORDZ_PWA_DRUM_SAAS_RESEARCH.md](../../docs/DISKLORDZ_PWA_DRUM_SAAS_RESEARCH.md) and future prompt generation.

**Target hardware workflow:** **Akai MPC Sample** (portable sampler) — WAV export at 44.1 kHz, USB-C / resample / file import.

## Build & run

No build step. Serve over HTTPS (or `localhost`) for PWA install:

```bash
cd disklordz/tr808-pwa
python3 -m http.server 8080
# open http://localhost:8080
```

Deploy: static host (Vercel, Cloudflare Pages) with root `disklordz/tr808-pwa`.

## Data flow

```text
UI (steps, knobs, pads) → Sequencer + accent → Voice engines (Web Audio)
    → Master saturation → destination (+ optional WAV export buffer)
```

Future: prompt API → generated samples → same pad map / export manifest for MPC Sample.

## Threading / realtime

All audio on **AudioContext** main thread; no `malloc` in hot path after init. Parameter changes via UI schedule on `currentTime`.

## Key modules

| File | Role |
|------|------|
| `index.html` | Single-file app (HTML + CSS + JS) |
| `manifest.webmanifest` | Installable PWA metadata |
| `README.md` | Circuit + UI feature list (product origin) |
| `docs/MPC_SAMPLE_WORKFLOW.md` | Phone → MPC Sample paths |

## Extension points

- Split DSP to `tr808-engine.js` + WASM later; share semantics with `vst-tr808/` plugin.
- Service worker for offline shell.
- SaaS: `/api/generate` hook + ZIP export preset `MPC_SAMPLE_44K_16PAD`.

## Related docs

- [../../docs/DISKLORDZ_PWA_DRUM_SAAS_RESEARCH.md](../../docs/DISKLORDZ_PWA_DRUM_SAAS_RESEARCH.md)
- [../../vst-tr808/REPO_HANDOFF.md](../../vst-tr808/REPO_HANDOFF.md) (pro circuit plugin — separate SKU)
