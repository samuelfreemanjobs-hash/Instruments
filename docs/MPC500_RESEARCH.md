# Akai MPC 500 — research notes (brainstorm prep)

Living research for the **MPC 500 vibe project**. External facts tagged **[assumed]** unless you confirm on hardware **[executed]**.

## Machine identity

| Item | Detail |
|------|--------|
| Era | ~2006 portable MPC |
| Pads | **12** (4×3) — editor profiles use 12-pad layout ([MPC Maid](https://github.com/cyriux/mpcmaid)) |
| Processor | SH-3 **7727** @ 100 MHz (same family as MPC 2500, lower clock) — [MAME driver notes](https://github.com/mamedev/mame/blob/master/src/mame/akai/mpc3000.cpp) |
| Storage | **Compact Flash** (samples, programs, sequences, OS) |
| Form factor | Battery-friendly **portable** workflow vs rack/desktop MPCs |

## Workflow (typical producer loop)

1. **Sample** — line in or load WAV from CF.
2. **Chop / trim** — truncate, normalize (in box or in computer).
3. **Program** — map samples to **pads** (velocity, tuning, layers per pad on supported models).
4. **Sequence** — 16-step grid (and variations) per track; swing and tempo.
5. **Perform** — pad mutes, overdubs, song mode (product-dependent).
6. **Export** — MIDI, WAV resample, or backup CF image.

Constraint mindset: **small screen, finite RAM/CF**, favor **mono drums**, short one-shots, and disciplined naming.

## File formats & tooling

| Tool | Role | MPC 500 relevance |
|------|------|-------------------|
| [MPC Maid](https://github.com/cyriux/mpcmaid) (Java, LGPL) | Drag WAV → pads; export **PGM + WAV**; slicing → MIDI groove | **Primary PC/Mac editor** with explicit **500 profile** |
| [Stephen Norum PGM docs](http://mybunnyhug.com/fileformats/pgm/) | Binary PGM structure (MPC1000 family) | Basis for Maid; 500-compatible exports |
| [Open500](https://github.com/M45K/open500) | OS 1.31 patch; add-ons as special WAVs (Chop Shop, Grid Seq, USB MIDI beta) | Optional **superpowers** if you accept patched OS |
| [SampleSlice](https://github.com/xyplex3/SampleSlice) | Transient/grid slice → **.xpm** (+ WAV) | Geared to **modern** MPC XML; path may differ for 500 — validate |
| [mpc-tools](https://github.com/p3r7/mpc-tools) | CLI PGM utils (2000XL lineage) | Reference for low-level PGM hacking |
| [AkaiMPCpgmTOiMPCPro2](https://github.com/superMDMArio/AkaiMPCpgmTOiMPCPro2) | PGM → iMPC Pro 2 | Shows PGM as interchange; not hardware loader |

**Practical v0 recommendation:** treat **MPC Maid + manual CF copy** as the proven bridge; automate later in `tools/mpc500/`.

## MIDI & hybrid setups

- Stock: **DIN MIDI** in/out for sync and triggering external gear.
- Open500 **USB MIDI (beta)** on OS 1.31: pads/seq to computer ([Open500 README](https://github.com/M45K/open500)) — useful for **vibe coding** with a DAW or custom JUCE receiver.
- Map pads to General MIDI drum notes when exporting grooves from Maid (compatibility with SP1200 default ch10 maps, etc.).

## Community / culture (vibe coding angles)

- **Portable beatmaking** — sketch anywhere; CF as “tape”.
- **12-pad discipline** — fewer voices → stronger sound choice (pairs well with Disklordz **mpc-ready-808** neutral packs).
- **Chop-first** — loops → slices → resequence (Open500 Chop Shop parallels SP1200 chop modal mentally).
- **OS extensions** — Open500 proves active 2026 hobbyist scene; respect GPL and **do not ship Akai OS** in repo.

## This monorepo — existing touchpoints

| Asset | Connection |
|-------|------------|
| `disklordz/website` preset **`mpc-ready-808`** | Marketing + generation tuned for MPC handoff |
| `disklordz/daw-inbox` | Auto-unzip kits to a stable folder for drag into Maid |
| `SP1200/` | Sampler + pad UX reference (different spec, same energy) |
| `disklordz/agents/profit/dsp-vst-architect` | Future pad-grid monitor plugin, USB MIDI test harness |
| `docs/DISKLORDZ_SAAS_V0.md` | ZIP manifest + MPC-oriented download copy |

## Idea backlog (for brainstorm)

**A. Content pipeline**

- Disklordz kit → inbox → Maid template (kick/snare/hat/clap slots 1–12).
- Batch rename + peak normalize to −0.3 dBFS before export.
- “CF pack” manifest JSON (program name, BPM, pad map).

**B. Software**

- Python **staging** CLI: folder → Maid-ready tree + README for CF copy.
- Read-only **PGM inspector** (leverage Norum docs).
- JUCE **12-pad MIDI surface** mirroring 500 layout for preview.

**C. Performance**

- Open500 Grid Seq + Disklordz loops at matched BPM.
- MIDI clock sync: MPC master → hardware synth; or DAW master → MPC.

**D. Product / Disklordz**

- SKU: “MPC500 Session Pack” with Maid project file + CF instructions.
- RAG lane doc for MPC500-specific prompt chips.

**E. Research spikes**

- Confirm your **OS version** and CF size **[executed on device]**.
- Capture one **PGM + WAV** set from Maid and store as golden fixture (no copyrighted samples).

## Risks & legal

- Do **not** commit Akai OS images, cracked tools, or proprietary PGM reverse-engineering dumps.
- Open500: GPL; Akai firmware stays user-local.
- Sample content: only user-owned or Disklordz-generated WAVs in repo fixtures.

## Next session prompts (copy into chat)

1. “I run OS ___ on MPC500; CF is ___ MB; I use ___ for samples.”
2. “First vibe: ___ genre, ___ BPM, chop vs one-shots.”
3. “Hybrid: MPC + ___ (DAW / SP1200 / synth).”

Update this doc as we **[execute]** tests on real hardware.
