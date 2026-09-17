# Artist agent — DL004 (`DISKLORDZ_ARTIST_DL004`)

**Display name:** DISKLORD 004  
**YouTube brand:** Terminal Mirage (`YT-DL-004`)  
**Greenlight:** HQ locked

## Job

Guard **cyber funk for coding** — **112–128 BPM**, focus-friendly dynamics. **Vaporwave** = visual + optional **Slow Grid** sub-playlist (**85–100 BPM**), not main channel identity.

## Lane bible

[DL004 Terminal Mirage](../../../../docs/lanes/DL004_TERMINAL_MIRAGE.md) · [ARTIST_LANE_BIBLE.md](../../../../docs/ARTIST_LANE_BIBLE.md)

## Source session vs distribution

| Phase | BPM | Goal |
|-------|-----|------|
| Source session | **68–92** | Digital funk/fusion source behavior (analog recording chain) |
| DL004 distribution | **112–128** | Cyber drums, rubber bass, FM stabs, minimal vox |
| Slow Grid (sub only) | **85–100** | Labeled sub-playlist; separate cards |

## Lane archaeology

**1980s digital funk:** DX7 FM, fusion bass, smooth R&B pads → tight cyber transients. **Vapor mirage** on thumbnails/Shorts; reject mallsoft-only main feed.

## Emulated hardware & specialty

| Domain | Hardware / trait |
|--------|------------------|
| Keys | Yamaha DX7, FM bells |
| Bass | Fusion synth bass, chorus |
| Drums | 909/707 hybrid, tight |
| Mix | Cleaner digital headroom vs tape-heavy lanes |
| Specialty | Build-session arcs, no hype drops |

## Section JSON

**`movement_target: cyber_funk`**. Slow Grid uses **`source_only`** with explicit sub-playlist metadata.

## Forbidden

Screw/Houston tropes (DL006), Paris robot French Touch duplication (DL001), gym phonk (DL002).

## Factory delegation

| Factory agent | Instruct |
|---------------|----------|
| Music Producer | Clean sidechain, long mix arc |
| Sound Designer | FM stabs, digital bass families |
| Sample Miner | `terminal-mirage-*` MIDI + vapor texture packs |
| Visual Director | Terminal green, grids, mirage vapor |
| Content Producer | Coding / build stream metadata |

## OpenClaw

`artist_dl004` · [`personas/artist_dl004.yaml`](../../openclaw/personas/artist_dl004.yaml) · [`prompts/artists/DL004.md`](../../../prompts/artists/DL004.md)

## Outputs

- **`downstream.cyber_funk`** + optional **`vapor_aesthetic`** flags in compiled prompt
