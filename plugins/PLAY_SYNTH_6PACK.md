# PlaySynth 6-Pack Producer Suite

Same **Play shell** on every instrument: equal-power **A/B blend**, **XY pad**, **Dice**, **8 macros**, **12 BinaryData presets**, `templates/juce8-synth-base/` scaffold.

| # | Plugin | CODE | Genre | Layer A | Layer B | Signature FX | UI |
|---|--------|------|-------|---------|---------|--------------|-----|
| 1 | AnalogDreams84 | Ad84 | 80s synthwave | Dual PWM/saw + sub | 80s WT + FM bell | Juno BBD + 80s plate | Magenta / blue |
| 2 | CloudSupplyTrap | CsTr | Trap / phonk bells | FM bell + 808 sub | Formant choir + crystal WT | Shimmer verb + ping-pong | Obsidian / cyan |
| 3 | LoFiGlowTape | LgTp | Lo-fi / VHS keys | EP tine / felt | Tape flute + vinyl | Wow + 12-bit + spring | Amber / charcoal |
| 4 | **PhonkCowbell94** | Pc94 | Memphis / drift phonk | **808 cowbell BP** + 808 sub | Dark choir + horror bell + hiss | 4-track cassette + bitcrush + crypt verb | Red / black / chrome |
| 5 | **FortyKeysSoul** | Fk40 | 40s / OVO R&B keys | Dark EP + warm sub | Underwater choir + swell bell | Half-speed shimmer + underwater LPF | Gold / bronze |
| 6 | **SoulSessions70** | Ss70 | 70s soul / Motown | B3 + clavinet | Solina strings + soul brass | Leslie + auto-wah + spring | Walnut / cream |

**Shared UI contract:** `plugins/PLAY_SYNTH_SHARED_UI.md` (v2: arp/chord, macro lock, scope FIFO, 2× OS dirt).

**Factory queues:**

- Long prompts: `disklordz/vst-factory/data/overnight-queue.json` — `suite: play_synth_6pack`, priority **10**
- Compact 6-pack JSON: `disklordz/vst-factory/data/play_synth_6pack_master.json`
- Repo map (Cursor vs blog advice): `docs/VST_AI_FACTORY_INDEX.md`

**Future slot (not queued):** Nacht / dark techno — APC stub TBD.

## Akai MPC Sample (Melodee) — same 6 engines, headless

Desktop targets live at **repo root** with WebView. **MPC Sample** ports use `vst/MpcPlay*` (aarch64 VST3, no editor, K1–K3 macro order).

| Doc / queue | Path |
|-------------|------|
| MPC 6-pack map + K1–K3 | `plugins/MPC_PLAY_SYNTH_6PACK.md` |
| Factory manifest | `disklordz/vst-factory/data/mpc_play_synth_6pack_master.json` |
| Headless contract | `disklordz/vst-factory/data/mpc_play_factory_contract.md` |
| Template | `templates/mpc-play-synth-base/` |
| Linux batch | `tools/run-factory-mpc-play6.sh` |

Legacy index: `PLAY_SYNTH_TRILOGY.md` → first three only.