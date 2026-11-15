# CHORD ENGINE — FL Studio quick start

**Product:** DiskLordz CHORD ENGINE (VST3 instrument)  
**Build:** `DiskLordz-Audio/` → `CHORD ENGINE.vst3`

## Install

1. Download the **chord-engine-vst3-windows-*** artifact from the latest green **DiskLordz Audio (Windows VST3)** GitHub Actions run (or build locally with `DiskLordz-Audio/build-windows.ps1`).
2. Copy `CHORD ENGINE.vst3` into  
   `C:\Program Files\Common Files\VST3\`  
   (or point FL Studio’s extra VST folder at the build output).
3. In FL Studio: **OPTIONS → Manage plugins → Find plugins** (scan).

## Load (recommended routing)

CHORD ENGINE is an **instrument** with a **built-in preview synth** so you hear chords immediately without extra routing.

1. Add an instrument slot in the **Channel rack** → **CHORD ENGINE**.
2. Pick **Progression** (Dark Trap, Drift Phonk, Memphis, etc.) and set **Root** + **Scale**.
3. Press **Play** on FL’s transport — the plugin follows host tempo and advances one chord per bar.
4. Or click **Audition** / enable **Loop audition** to hear the loop without starting a full project arrangement.

**Optional MIDI capture:** enable **MIDI to piano roll** to emit chord notes to the host (stack with a sampler on the same channel if you want your own sound).

## First progression in under a minute

| Step | Control |
|------|---------|
| 1 | Progression → **Dark Trap — tension loop** |
| 2 | Root → **F** · Scale → **Natural minor** |
| 3 | Press **Play** (140 BPM default if host idle) |
| 4 | Tweak **Spread** / **Inversion** for voicing |

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| No sound | Confirm the channel is not muted; use **Audition**; check FL master volume. |
| Silence on old build | Pre-alpha **Milestone 001 shell** produced no MIDI/audio — update to a build that includes Stage C+ preview synth. |
| Stuck notes | Stop transport; toggle Audition off (plugin sends all-notes-off on stop). |

## State / presets

- Plugin **programs** map to starter progressions (host program menu).
- Project save restores parameters via VST3 state.

Full PRD: [CHORD_ENGINE_PRD.md](CHORD_ENGINE_PRD.md)