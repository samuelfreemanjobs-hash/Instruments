# Serum autonomous preset synthesis — research index

Condensed reference from the Serum binary / agent pipeline research brief. Use with **`serum_forge`** symbolic layer and external **serum-preset-packager** / **PySerum** for binary output.

## Non-goals

- **DX7 SysEx → Serum** is invalid (6-op PM vs wavetable subtractive). `serum_forge.dx7.reject_dx7_sysex` enforces this at ingestion boundaries.

## Container formats

| Format | Magic / header | Payload |
|--------|----------------|---------|
| `.fxp` | Steinberg `CcnK`, plugin id `XfsX` | Opaque Serum 1 state |
| `.SerumPreset` | `XferJson\\0` + uint64 JSON len + UTF-8 metadata | zstd CBOR (~600 params) |

Wavetables: 2048 samples × up to 256 frames per osc; optional RIFF `clm ` chunk for frame metadata.

## Five-stage pipeline (implemented in `serum_forge.pipeline`)

1. **Intent** — NL prompt or target audio objective  
2. **Symbolic assembly** — `SerumSymbolPatch` / ParamSpec + guardrails  
3. **Binary compile** — external packager (`SERUM_PACKAGER_BIN`)  
4. **Headless render** — Pedalboard + `SERUM_VST3_PATH` (preset chunk reload, not full automation bus)  
5. **Perceptual eval** — CLAP vibe score + mel/MFCC losses (stubs in `perceptual.py`; extend in prod)

## Host automation bottleneck

VST parameter automation **does not** expose mod matrix, custom LFO curves, or embedded wavetable PCM. Batch systems must **write `.SerumPreset` / FXP** and reload plugin state.

## Methodologies (roadmap)

| Method | Role in repo |
|--------|----------------|
| Constrained symbolic (LLM + OpenAPI) | **Now:** `param_spec.py`, Gem doc |
| VAE latent presets | Future corpus + trainer |
| CMA-ES inverse design | Future + headless render loop |
| Spectrogram CNN estimator | Future |

## Guardrails (implemented)

See `guardrails.py`: unison headroom \(-10\\log_{10}(N)\), reverb wet ≤ 40%, resonance ≤ 0.85, FM warp depth cap, archetype tables (bass / pluck / pad).

## Gemini Gem

Copy-paste instructions: [GEM_SERUM_FORGE.md](GEM_SERUM_FORGE.md)

## Related tooling (external OSS)

- serum-preset-packager, node-serum2-preset-packager, PySerum  
- Spotify **pedalboard**, **DawDreamer** for headless VST3
