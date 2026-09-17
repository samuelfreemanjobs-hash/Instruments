# Sonic archaeology — Screw, Memphis, and phonk source design

DiskLordz **source compositions** are engineered so downstream Southern hip-hop workflows (chop, screw, phonk) behave like real vinyl archaeology — not generic “vintage vibes.”

**Communication rule:** Band members output **structured JSON** (+ optional Markdown notes). Isaac (Maestro) **merges** into one **high-density compiled prompt** with no semantic drift. Schemas: [`dl_section_payload.json`](../disklordz-factory/schemas/dl_section_payload.json), [`compiled_music_prompt.json`](../disklordz-factory/schemas/compiled_music_prompt.json).

## Analog source perspective (collective default)

Every OpenClaw sectional agent generates from a **late-twentieth-century analog perspective** (Neve/Studer chain, hardware listed in [OPENCLAW_PERSONAS.md](OPENCLAW_PERSONAS.md)). Modern EDM, hyper-compression, and autotune aesthetics are out of scope for **source sessions** (68–92 BPM per Isaac).

## Three downstream movements (one source crate)

All three movements sample **1970s–1980s soul, jazz-funk, quiet storm, electro-funk** — but **processing stories differ**:

| Movement | HQ artist lane | YouTube brand | Technical story |
|----------|----------------|---------------|-----------------|
| **Chopped & screwed** | **DL006** | Disklordz | DJ Screw: dual **SL-1200**, **−10% to −25%** pitch, ~16–25 RPM feel; R&B/quiet storm/electro-funk clarity under slowdown |
| **Memphis cassette phonk** | **DL002** (fast) + source for DL006 | Midnight Circuit / Disklordz | **DR-660/DR-5**, budget samplers; **SP-1200** 12-bit @ 26.04 kHz, 45 RPM sample → pitch down; orchestral soul vs frantic hat rolls |
| **Cloud / trill phonk** | **DL002** | Midnight Circuit | DJ Smokey lineage: modal jazz-funk, library music, Rhodes; **sidechain, bitcrush, tape-stop, digital LP filter** over 808 syncopation |

Factory **does not** impersonate named recordings — agents model **behaviors** (harmonic clarity under pitch-down, SP-1200 grit, Smokey-style ducking).

---

## DJ Screw (Houston) — frequency & repertoire behavior

- **Workflow:** Dual Technics **SL-1200**; pitch **−10% to −25%**; playback toward **16–25 RPM** effective feel.
- **Spectral effect:** High-mid synth stabs, slap bass, clean rhythm guitars → **thick low-mid**; vocal formants → **chest-heavy baritone**; wow/flutter and surface noise → **stretched atmosphere**.
- **Source selection bias:** Sophisticated **analog synth arrangements**, **polished vocal harmonies** that stay intelligible when slowed (behavior class: quiet storm, electro-funk, contemporary R&B c. 1980s — *not* sample lists of copyrighted masters).
- **Agent assignment:** Bernard/Evelyn/M Marcus optimize **DL006** briefs; Isaac compiles screw-ready source prompts.

---

## Memphis underground (1990s) — hardware poverty & contrast

- **Workflow:** **Boss DR-660**, **DR-5**; **Gemini DS-1224** / occasional **E-mu SP-1200**.
- **SP-1200:** 12-bit, 26.04 kHz, **SSM2044** LP filter → ring-aliasing, mid grit, punchy transients; **45 RPM sample → internal pitch down**.
- **Aesthetic:** Melancholic **orchestral soul loops** vs **frantic syncopated 808-style hat rolls** and deep sine kicks — “Playa Pimp” **lavish soul vs raw percussion**.
- **Agent assignment:** Bernard (DR-660 patterns, negative space); Marcus (orchestral minor soul voicings); Evelyn (bittersweet topline tropes); **DL002** for fast grid; stems also feed **DL006** screw when HQ approves.

---

## DJ Smokey — bridge to modern phonk

- **Workflow:** Modal **jazz-funk**, library cues, **quiet storm Rhodes** → heavy **sidechain ducking**, **bitcrush**, **tape-stop**, **resonant digital LP filter**; glides over **syncopated 808**.
- **Source must include:** Airy **Rhodes**, **suspended minor** harmony, expressive **woodwind-like** lines (synthetic behavior, not named lifts).
- **Agent assignment:** **DL002** Midnight Circuit; Smokey-style processing notes in `compiled_music_prompt.downstream.cloud_phonk`.

---

## OpenClaw merge pipeline

```text
Marcus  → keys_section.json     ─┐
Bernard → rhythm_section.json   ├─→ Isaac: merge → compiled_music_prompt.json
Evelyn  → vocal_section.json    ─┘         → external engine / DL-BRF / Lyria-class text
```

Maestro **rejects** free-form prose from forks; only schema-valid section payloads.

## Related

- [OPENCLAW_PERSONAS.md](OPENCLAW_PERSONAS.md)
- [OPENCLAW_GATEWAY_ROUTING.md](OPENCLAW_GATEWAY_ROUTING.md)
- [FACTORY_VINTAGE_COLLECTIVE.md](FACTORY_VINTAGE_COLLECTIVE.md)
- [DISKLORDZ_SP1200.md](DISKLORDZ_SP1200.md) (Labs authenticity for SP-1200 narrative)
