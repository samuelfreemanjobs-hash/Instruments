# MPC500 + PC only (your rig)

**Version:** 1.0 · **Status:** **Active setup** (2026-11-03)  
**Full stack doc (SparkLE optional):** [MPC500-SPARKLE-PC-STACK.md](MPC500-SPARKLE-PC-STACK.md)

You have **Akai MPC500** + **Windows PC** — no SparkLE required. The 808 Lab pipeline is unchanged; **manifest format** still uses `spark_le` pad names (format label, not hardware).

---

## Roles

| Piece | Job |
|-------|-----|
| **MPC500** | Chop, sequence, play samples, record, MIDI out to PC |
| **PC** | Generate kits (`npm run export:sprint-1-3`), `phonk_engine`, future chop/synth/screw apps, optional DAW |

**Control on PC without SparkLE:** mouse/keyboard in apps, **MPC pads → MIDI notes** into `phonk_engine`, or a cheap USB MIDI keyboard later.

---

## Signal flow (minimal)

```text
PC: factory / export:sprint-1-3 → WAV folders + spark-le-manifest.json
         │
         ├─► Copy OneShots/ into MPC500 (USB or CF workflow)
         │         └─► Pads 1–8 per manifest · sequence · mixdown
         │
         └─► phonk_engine.py --kit …\OneShots  ◄── MIDI from MPC500 (optional)
                    └─► audio out → speakers / interface
```

**MIDI** = trigger + clock. **Audio** = you must wire PC output to speakers (or sample PC output into MPC if your interface allows).

---

## Sessions 1–3 on MPC500 (today)

1. Pull packs from [sprint-kits/](sprint-kits/) (or regenerate on PC):

   ```bash
   cd disklordz/website && npm run export:sprint-1-3
   ```

2. Copy each `OneShots/*.wav` into MPC500 sample memory (your usual import path).
3. Assign **pad bank A, pads 1–8** to match manifest roles (sub, knock, snare, clap, hats, rim, FX).
4. Set sequence **BPM** from manifest (140 / 72 / 86).
5. Program or play; **export mix** from MPC (or resample to PC).
6. Score sessions in [reviews/](reviews/); keepers **N** until Vibe/Groove ≥ 4.

| Session | BPM | Folder |
|---------|-----|--------|
| 1 | 140 | `session-1-gangsta-trap/` |
| 2 | 72 | `session-2-dj-screw/` |
| 3 | 86 | `session-3-smooth-jazz-rap/` |

Reference loops in each pack’s `Loops/` folder — load as samples or A/B in MPC.

---

## PC engine without SparkLE

```powershell
cd disklordz\phonk-box\engine
python -m venv .venv
.venv\Scripts\activate
pip install -r requirements.txt
python phonk_engine.py --self-test
```

**Trigger from MPC500:** connect **MIDI out (MPC) → MIDI in (PC interface)**, enable MIDI track on MPC to send notes, run:

```powershell
python phonk_engine.py --kit C:\path\to\sprint-kits\session-1-gangsta-trap\OneShots --midi "Your MIDI In Port Name"
```

Rename kit files to `kick.wav`, `snare.wav`, … if needed — see [MIDI-MAP.md](../../../disklordz/phonk-box/protocol/MIDI-MAP.md). Long-term: manifest import so names match automatically.

**Parameters (no SparkLE knobs):** edit lane in factory prompt, or add PC UI later; CC 70–73 can come from MPC Q-Link only if you map them in a DAW — not required for sprint.

---

## When you add gear later

| Added | Use |
|-------|-----|
| **Audio interface** | Stable PC + MPC monitoring; low-latency `phonk_engine` |
| **USB MIDI keyboard** | Extra triggers / CC for PC synths |
| **Arturia SparkLE** | Optional dedicated knob/pad surface — [SPARKLE-MIDI-AND-CONTROLLER.md](SPARKLE-MIDI-AND-CONTROLLER.md) |

---

## Related

[808-LAB.md](../808-LAB.md) · [SPRINT-PROGRESS.md](SPRINT-PROGRESS.md) · [SOP-013](../sops/SOP-013-spark-le-phase-1-feed-spark.md)
