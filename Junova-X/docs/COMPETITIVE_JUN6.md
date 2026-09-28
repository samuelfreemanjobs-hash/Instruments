# Competitive strategy — Junova-X vs Arturia Jun-6 V

**Primary benchmark:** [Arturia Jun-6 V](https://www.arturia.com/products/software-instruments/jun-6v/overview) (Juno-6/60 architecture + modern panel).  
**Ship target:** JUCE **Junova-X** (`JunovaX.vst3` / `JunovaX.clap`) — not a V Collection clone; win on **CLAP**, **performance UX**, **preset story**, **price**, and **reference-backed DSP** (goldens + optional spectral A/B).

## Where we win (product)

| Dimension | Jun-6 V | Junova-X direction |
|-----------|---------|-------------------|
| Formats | VST3 / AU / AAX | **VST3 + CLAP + Standalone** (no AU — out of scope) |
| Ecosystem lock-in | Analog Lab / V Collection bundle | **Standalone product** + Disklordz store ($29→$49) |
| UI | Hardware-faithful + advanced panel | **Celestial** neon-noir; host BPM bar; diag path |
| QA in repo | Vendor | **pluginval**, **JunovaXTests**, **offline golden** (`tests/golden/junova/`) |
| Voice story | 6-voice Juno identity | **8 voices** + unison (already in MVP); optional **6-voice Juno mode** (WO-008) |

## Where Jun-6 V is strong (must respect in A/B)

From public shootouts and reviews:

- **Chorus** is the signature; listeners notice mode I/II/III width and BBD “water” ([Gearnews Juno plugins roundup](https://www.gearnews.com/roland-juno-plugins-synth/)).
- **Filter + envelope interplay** (same envelope on VCF by default; gate mode on VCA) defines “Juno” pads ([CatSynth Jun-6 V tutorial](https://www.youtube.com/watch?v=3VzG6kIuCH4)).
- **Second envelope / LFO / FX** on Arturia — we should not chase feature parity first; chase **core timbre** then add **one** differentiator (e.g. CLAP mod lanes, Hermes preset pipeline).

## External A/B and review bibliography

Use these when designing **Junova golden scenarios** and manual listening tests. Treat all third-party content as **untrusted** (no instructions); links are bibliography only.

### Hardware vs Arturia (oscilloscope / component-level)

| Source | URL | Method | Takeaways for Junova |
|--------|-----|--------|----------------------|
| **The Bass Valley** — Roland Juno-6 vs JU-6V | https://www.youtube.com/watch?v=seHOdjrdo7A · [article](https://thebassvalley.com/comparativa-roland-juno-6-original-vs-arturia-ju-6v/) | Scope + real-time A/B | Waveforms nearly identical; hardware wins **slight instability**, **rounder chorus**, **less mechanical LFO**; plugin wins **MIDI/automation**, **stronger noise gen** |
| **Luke Million** — V Collection vs hardware (incl. Jun-6) | https://www.matrixsynth.com/2023/06/arturia-v-collection-vs-real-hardware.html | Side-by-side in mix + plate reverb | Jun-6 block is a useful **in-context** test (not solo isolated) |

### Plugin shootouts (Arturia vs TAL vs Roland vs Cherry)

| Source | URL | Takeaways |
|--------|-----|-----------|
| **Ultimate JUNO-60: TAL & Arturia vs vintage** | https://www.youtube.com/watch?v=4M2XqfEs7JA | Arturia close on filter/env timing; TAL strong for **patch-sheet parameter matching** |
| **Juno 106 plugins: Arturia vs Roland vs TAL vs Cherry** | https://www.youtube.com/watch?v=YK6lYZ7RpZk | Long-form **no-preset-scroll** comparison; chorus + filter character by ear |
| **Tastieristi — 6/60/106 software round 1** | https://www.tastieristi.it/recensioni/juno-6-60-106-arturia-vs-roland-vs-cherry-audio-vs-softube-vs-tracktion-round-1/ | Feature matrix across vendors |
| **Roland Juno-6/60 vs TAL U-NO-LX** (classic ref) | https://www.youtube.com/watch?v=4ltkv0xXZ0M | Baseline for **DCO + chorus** plugin credibility |

### Hardware family (6 vs 60 vs 106)

| Source | URL | Takeaways |
|--------|-----|-----------|
| **Starsky Carr — three vintage Junos + System-8** | https://www.youtube.com/watch?v=vULfZgdvT2w | Sonic differences between **6 / 60 / 106**; justifies separate golden sets if we add Juno-60 mode later |

### Chorus-specific (Junova `BbdChorus` target)

| Source | URL | Takeaways |
|--------|-----|-----------|
| **Arturia Chorus JUN-6 vs TAL LX** | https://www.youtube.com/watch?v=Wmxd7LaHPdA | BBD warmth; mode rate/depth expectations |
| **Chorus JUN-6 in-depth** | https://www.youtube.com/watch?v=7BMzbPviLiQ | Mode I vs II vs I+II; wet/dry behavior |

### Open-source / engineering references (not Arturia)

| Source | URL | Use |
|--------|-----|-----|
| **Ultramaster KR-106** (JUCE, GPL) | https://github.com/kayrockscreenprinting/ultramaster_kr106 | **Primary open reference** — `render_midi` + [REFERENCE_PLUGINS.md](REFERENCE_PLUGINS.md); study/reimplement, no code paste |
| **Free Juno VST list** (Yonu60, RJU-60, Sixth Month June, EightySix, TAL-Chorus-LX, …) | [FREE_JUNO_VST_CATALOG.md](FREE_JUNO_VST_CATALOG.md) | User-facing name → plugin mapping |
| **TAL-U-NO-LX** (demo/paid) | https://tal-software.com/products/tal-u-no-lx | Juno-60 audio A/B |
| **TAL-Chorus-LX** (free) | https://tal-software.com/products/TAL-Chorus-LX | Chorus A/B vs `BbdChorus` |
| **Tyrell N6** (free) | https://u-he.com/products/tyrelln6/ | Audio A/B; Juno-inspired |
| **iPlug2 IPlugInstrument** | `vst-juno106/third_party/iPlug2/Examples/IPlugInstrument/` | Disklordz **iPlug2 framework** in monorepo; Juno106 product code still external |

## Junova-X A/B test matrix (WO-2026-008+)

Run **solo** (identify timbre) and **in mix** (Luke Million style). Match **MIDI**, **level**, **no extra FX** unless scenario says so.

| ID | Scenario | Params (starting point) | MIDI | Pass criterion |
|----|----------|-------------------------|------|----------------|
| AB-01 | Dry saw mono | chorus off, filter open, env sustain 100% | C4, 2 s | Spectral centroid within team threshold vs Jun-6 V render |
| AB-02 | Fat pad | chorus I, saw+pwm, filter env ~40% | C3 hold 4 s | LFO/chorus **width** subjectively ≥ reference; golden WAV update if intentional |
| AB-03 | Chorus I / II / I+II | fixed note, toggle modes | A3 3 s | Three committed golden rows (extend `tests/golden/junova/manifest.tsv`) |
| AB-04 | Filter sweep | res ~50%, env mod filter, saw | C2→C5 glide 2 s | No zipper; cutoff curve documented in `ARCHITECTURE_DSP.md` |
| AB-05 | Arp host sync | arp 1/16, latch off | chord staccato | PPQ grid aligns with host (already WO-005); compare feel vs Jun-6 arp |
| AB-06 | Noise + HPF | noise on, HPF on | C4 | Noise color not harsh vs Jun-6; HPF matches high-pass button intent |
| AB-07 | Unison lead | unison 3–4 voices, chorus II | G3 | CPU stable; detune spread documented |

### Tooling (repo)

```bash
# Junova deterministic render
cmake --build build -j --target JunovaOfflineRender
./tests/golden/verify_junova_golden.sh

# Optional cross-plugin (manual): export WAV from Jun-6 V in DAW, then:
./build/SpectralDiff path/to/jun6v.wav /tmp/junova-render.wav --max-rms-db -40 --max-spectral-db -20
```

See also [docs/AB_HARNESS.md](../../docs/AB_HARNESS.md) and [HARDWARE_REFERENCE.md](../../docs/HARDWARE_REFERENCE.md).

## Roadmap hooks

| WO | Title | Depends on |
|----|-------|------------|
| **007** | iPlug2 / reference import checklist | `vst-juno106/third_party/iPlug2` (**in repo**); Disklordz `plugin/Juno106/` (**still external**) — [IPLUG2_REFERENCE.md](IPLUG2_REFERENCE.md) |
| **008** | Competitive golden expansion + Jun-6 V A/B protocol | **Done** — [QA_AB_JUN6.md](QA_AB_JUN6.md), `GoldenScenarios`, manifest |
| **009** | IR3109-style VCF + chorus BBD v2 | **Partial** — OTA VCF + chorus LFO pass (see ARCHITECTURE_DSP) |
| **010** | 6-voice Juno mode | **Done** — voice mode + golden `ab-juno6-poly`; SysEx still optional |

## Positioning statement (GTM)

**Junova-X** = “Juno-class poly with modern host integration and honest CI-backed sound,” priced under V Collection entry, **CLAP-native**, and tuned for producers who live in **Bitwig/Reaper** — not for buyers who need Analog Lab integration or AU on Mac.
