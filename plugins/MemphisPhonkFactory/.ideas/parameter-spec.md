# Memphis Phonk Factory — parameter spec

| ID | Display | Type | Range | Default | Notes |
|----|---------|------|-------|---------|-------|
| `mpf_loop_bpm` | Loop BPM | float | 90–140 | 118 | Preview + default loop export tempo |
| `mpf_swing` | Swing | float | 0–0.08 | 0.03 | MPC 16th swing |
| `mpf_drive` | Tape drive | float | 1.5–4.5 | 2.5 | Cassette saturation amount |
| `mpf_hiss` | Tape hiss | float | 0–0.02 | 0.004 | Noise floor |
| `mpf_variation` | Variation | int | 1–4 | 1 | Kit timbre offset |
| `mpf_preview` | Loop preview | bool | off/on | on | Audition loop in plugin |
| `mpf_master` | Master | float | 0–1 | 0.85 | Output level |

| `mpf_batch_count` | Batch kits | int | 1–16 | 4 | Kits per batch export |

Pattern (UI + state props `kickMask` / `snareMask` / `openHatMask`): 16-step kick, snare, open hat.

Export actions: **Export kit**, **Batch export** (timestamped `Memphis_Phonk_Batch_* / Kit_NNN /`).
