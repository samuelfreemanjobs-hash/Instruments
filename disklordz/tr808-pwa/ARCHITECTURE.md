# TR-808 PWA (Disklordz)

## Purpose

Single-file **Progressive Web App**: playable **Roland TR-808**–style virtual analog drum machine in the browser. Lead-magnet / SaaS front-end; **separate SKU** from **NovaDrum** (`vst-tr808/` plugin) and **Junova-X**.

## Build & run

No build step. Serve over HTTPS (or `localhost`) for PWA install:

```bash
cd disklordz/tr808-pwa
python3 -m http.server 8080
# open http://localhost:8080
```

Deploy: static host (Vercel, Cloudflare Pages) with root `disklordz/tr808-pwa`.

**Note:** Full app files (`index.html`, `variants/`, `manifest.webmanifest`) are on branch **`cursor/tr808-pwa-app-94ae`** until merged. This doc on `main` is the architecture anchor for agents.

## Data flow

```text
UI (steps, knobs, pads) → Sequencer + accent → Voice engines (Web Audio)
    → Master saturation → destination (+ optional WAV export buffer)
```

Future: prompt API → generated samples → same pad map / export manifest for MPC Sample.

## Threading / realtime

All audio on **AudioContext** main thread; no allocation in hot path after init. Parameter changes via UI schedule on `currentTime`.

## Key modules (on feature branch)

| File | Role |
|------|------|
| `index.html` | Launcher — pick Disklordz vs original build |
| `variants/disklordz.html` | Repo-maintained single-file app |
| `variants/original.html` | Original single-file build |
| `manifest.webmanifest` | Installable PWA metadata |
| `README.md` | Circuit + UI feature list |
| `docs/MPC_SAMPLE_WORKFLOW.md` | Phone → MPC Sample paths |

## Extension points

- Split DSP to `tr808-engine.js` + WASM later; share **semantics** with `vst-tr808/` only via explicit WO.
- Service worker for offline shell.
- SaaS: `/api/generate` hook + ZIP export preset `MPC_SAMPLE_44K_16PAD`.

## Related docs

- [docs/TR808_JUNOVA_PRODUCT_MATRIX.md](../../docs/TR808_JUNOVA_PRODUCT_MATRIX.md)
- [disklordz/website/ARCHITECTURE.md](../website/ARCHITECTURE.md) — drum kit SaaS (different product)
- NovaDrum plugin: [vst-tr808/README.md](../../vst-tr808/README.md)
