# Free & “free-ish” Juno VST catalog (user reference list)

Names below map common spellings / voice-to-text to **real plugins**. Use for **audio A/B** (export WAV → `compare_jun6_reference.sh`) unless marked **open-source** (study + `render_midi` where available).

**We do not** ship or decompile third-party binaries in this repo.

| You said | Plugin (correct name) | Juno target | Cost | Formats / OS | Junova use |
|----------|----------------------|-------------|------|--------------|------------|
| **Yonu106** | [**Yonu60**](https://plugins4free.com/plugin/720/) (L-Day) | Juno-*inspired* (often listed in “106” roundups) | **Free** | VST **32-bit** Windows | Audio A/B; bridge in 64-bit DAW |
| **tal-un-no62** | [**TAL-U-NO-LX**](https://tal-software.com/products/tal-u-no-lx) | Juno-**60** | **Demo** (periodic noise) / **paid** full | VST2/3, AU, AAX, CLAP | Audio A/B only; not open-source |
| **June 21** | *(unclear name)* — treat as **Juno-106** row | 106 | — | — | Use **KR-106** or capture from your installed 106 emu |
| **Juno 6** | [**EightySix**](https://www.morphoice.com/eightysix) (Morphoice) | Juno-**6** circuit model | **Free** (Gumroad PWYW) | VST3, AU · Win/Mac/Linux **64-bit** | Audio A/B; includes **EightySix Chorus** FX |
| **sixth month June** | [**Sixth Month June**](https://freevstplugins.net/sixth-month-june/) (Elektrostudio) | Juno-**6** | **Free** | VST **32-bit** Windows | Audio A/B |
| **rju-60** | [**RJU-60**](https://plugins4free.com/plugin/1935/) (EFM) | Juno-**60** | **Free** | VST **32-bit** Windows | Audio A/B |
| **chorus jun-6** | [**TAL-Chorus-LX**](https://tal-software.com/products/TAL-Chorus-LX) | Juno-60 **chorus** | **Free** (demo noise/min) | VST3, AU, CLAP, Linux | **Best free chorus A/B** vs `BbdChorus` |
| *(same)* | [**Arturia Chorus JUN-6**](https://www.arturia.com/products/software-instruments/chorus-jun-6/overview) | Juno **chorus** | **Paid** (was free Dec 2020 if claimed) | VST3, AU, AAX | Audio A/B if you own it |

## Also use (106 / modern 64-bit)

| Plugin | Notes |
|--------|--------|
| [**Ultramaster KR-106**](https://github.com/kayrockscreenprinting/ultramaster_kr106) | **Free + GPLv3** · 6/60/106 · VST3/CLAP · `setup_kr106_reference.sh` |
| [**Tyrell N6**](https://u-he.com/products/tyrelln6/) | Free · Juno-*inspired* · not 106-accurate |
| [**TAL-Chorus-LX**](https://tal-software.com/products/TAL-Chorus-LX) | Free chorus · pair with any dry Junova render |

## A/B workflow (any row above)

1. Match Junova scenario: `./build/Junova-X/JunovaOfflineRender /tmp/jx.wav --scenario ab03-chorus-i 57 100 3.0 44100`
2. Same MIDI note, length, **no extra FX** in the reference plugin.
3. Compare:
   ```bash
   ./tests/golden/junova/compare_jun6_reference.sh /path/to/reference.wav ab03-chorus-i 57 3.0
   ```
4. Optional stash: `tests/golden/junova/reference/` (WAV gitignored).

### Chorus-only check (TAL-Chorus-LX vs Arturia Chorus JUN-6)

Render Junova **dry** (`ab01-dry-saw`), bus through reference chorus plugin in DAW (modes I / II / I+II), export stereo WAV, compare to `ab03-chorus-*` scenarios.

## 32-bit legacy plugins (Yonu60, RJU-60, Sixth Month June)

- **Windows:** 32-bit host or bridge (jBridge, Reaper bridge, etc.) on 64-bit DAWs.
- **Linux / Cloud Agent:** use the in-repo **Wine win32 sandbox** — [`Junova-X/sandbox/win32-vst-host/`](../sandbox/win32-vst-host/README.md) (`setup.sh`, `run_sandbox.sh`, drop DLL + VSTHost.exe). Not in CI by default.

## TAL-U-NO-LX vs “free”

Unregistered demo is **not** production-free (noise bursts). Still useful for **short** A/B clips if you accept demo limitations. Full license is paid.

## Related

- [REFERENCE_PLUGINS.md](REFERENCE_PLUGINS.md) — legal policy + KR-106
- [QA_AB_JUN6.md](QA_AB_JUN6.md)
- [COMPETITIVE_JUN6.md](COMPETITIVE_JUN6.md)
