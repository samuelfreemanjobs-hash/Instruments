# APC workflow (Noizefield-compatible)

This repo implements the **Audio Plugin Coder** phase system described in
[Noizefield’s walkthrough](https://www.noizefield.com/music-news-tv/make-professional-vst-audio-plugins-with-zero-money-with-ai-audio-plugin-coder-walkthrough-part-1)
and [Part 2 — Dream](https://www.noizefield.com/news/ai-vst-plugin-development-audio-plugin-coder-part-2).

Upstream reference: [github.com/Noizefield/audio-plugin-coder](https://github.com/Noizefield/audio-plugin-coder)

## Video series → commands

| Part | Topic | Command |
|------|--------|---------|
| 1 | Setup, toolchain, paths | `/apc-setup` |
| 2 | Dream, brief, parameters, reference audio | `/apc-dream VinylNoise` |
| 3 | Plan, architecture, WebView vs native | `/apc-plan VinylNoise` |
| 4+ | Design, implement, test, ship | `/apc-design` → `/apc-impl` → `/apc-test` → `/apc-ship` |

## ADHD quick path (Cursor + WSL)

```bash
cd ~/code/Instruments
git pull
git checkout cursor/nts1-coder-scaffold-91dc   # until merged to main

bash bin/apc setup
```

In Cursor chat:

```
/apc-dream MyPlugin
```

(paste your sonic description + parameters)

```
/apc-plan MyPlugin
/apc-design MyPlugin
/apc-impl MyPlugin
/apc-test MyPlugin
/apc-ship MyPlugin
```

Check progress anytime: `/apc-status MyPlugin`

## Folder map

| Path | Role |
|------|------|
| `plugins/<Name>/` | APC **state** + `.ideas/` specs |
| `vst/<Name>/` | **JUCE** implementation |
| `release/<Name>/` | Shipped `.vst3` copies |
| `reference-audio/` | Optional global samples |
| `plugins/<Name>/references/` | Per-plugin samples (Part 2) |

## vs Noizefield APC repo

| Noizefield | Instruments |
|------------|-------------|
| `plugins/` | Same |
| `_tools/JUCE` | `vst/JUCE` submodule |
| Visage / WebView | **juce-native** default (WebView optional later) |
| `node bin/apc.js` | `bash bin/apc` + `tools/new-vst.sh` |
| NTS-1 | Optional via `logue-hardware-port` skill |

## Architecture depth

After **plan**, read [dsp-architecture-specification.md](dsp-architecture-specification.md) for program chain, surface chain, global, and complexity tiers.

## CLI

```bash
bin/apc setup
bin/apc init MyPlugin
bin/apc status MyPlugin
bin/apc paths
bin/apc build MyPlugin
bin/apc check
```
