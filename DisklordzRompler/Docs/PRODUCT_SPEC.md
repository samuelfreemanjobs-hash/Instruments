# DISKLORDZ ROMPLER — product spec (P0)

## Vision

Commercial-scale **workstation-style** rompler: large **raw PCM ROM library** → multisamples → tones → **4-tone patches** → **1,000+ factory presets** (mostly variations on ~250–350 source multis). Dark trap / phonk / cyber-shift identity via **CYBER_SHIFT** ROM layer + texture stacks.

See [ROM_FACTORY.md](ROM_FACTORY.md) for the full production pipeline.

## P0 (current slice)

- 16-card DLRROM01 library + ROM Factory tooling (manifest → WAV → `.dlrrom`)
- 24-voice engine, 4 tone layers per note (`toneRomBank` + `toneProgram`)
- 8 macros + tone levels + filter + amp ADSR
- 8 factory presets (catalog schema ready for 1,000+)
- MAIN-style editor (macros + tones + preset)
- VST3 + Standalone, APVTS state

## P1

- Full tab UI (BROWSER, TONE, MOD, FX, …)
- User sample pack load
- Mod matrix, insert FX
- CLAP

## Non-goals

- WAVE-909 code or wavetable engine
- HISE export as runtime
- Shipping commercial sample libraries in git
