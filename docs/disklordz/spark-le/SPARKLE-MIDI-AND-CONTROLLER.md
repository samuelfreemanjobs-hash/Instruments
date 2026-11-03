# SparkLE as controller vs DIY hardware

**Version:** 1.1 · Companion to [808-LAB-HYBRID.md](808-LAB-HYBRID.md) and [phonk-box/ARCHITECTURE.md](../../../disklordz/phonk-box/ARCHITECTURE.md).

## Decision (locked)

**Controller: Arturia SparkLE** — standalone primary; USB MIDI to the PC engine when needed. **No DIY Phonk Box build** unless SparkLE limits block a specific workflow (document the gap first).

| Date | Decision |
|------|----------|
| 2026-11-03 (America/Detroit) | SparkLE is the 808 Lab hands; phonk-box tree = optional PC engine + deferred DIY only |

## Short answer

**Yes — for most DiskLordz / 808 Lab work, SparkLE is the better “controller” than a $100 DIY box.**

You already have **8 velocity pads**, **knobs**, **USB**, and **standalone playback**. That matches our [8-pad law](../sops/SOP-012-spark-le-elite-system-charter.md). Building duplicate arcade buttons only makes sense if you want a **second**, **offline**, or **custom-step** rig.

---

## Three modes (pick one primary)

| Mode | What happens | Best for |
|------|----------------|----------|
| **A. SparkLE standalone** | Load DiskLordz one-shots/loops **into SparkLE**; play and sequence on device | **Default 808 Lab** — Sessions 1–5, keepers, phonk/Memphis/screw |
| **B. SparkLE USB MIDI → PC engine** | SparkLE sends pad/knob MIDI; **phonk_engine** (or DAW) makes sound | Custom sequencer, lane DSP, experiments beyond SparkLE UI |
| **C. DIY Phonk Box USB MIDI** | Teensy/RP2040 → same PC engine | No SparkLE at gig; learn hardware; extreme custom layout |

**Operating plan:** Run **A** for Sessions 1–5 and keepers. Add **B** only when you outgrow SparkLE’s pattern tools. **C** is out of scope unless you reopen hardware ([ENCLOSURE doc](../../../disklordz/phonk-box/docs/ENCLOSURE-AND-CONNECTORS.md) stays deferred).

---

## Mode A — standalone (preferred)

```text
Disklordz factory / manifest → OneShots + Loops → SparkLE user samples → pads 1–8
```

- **No** separate phonk_engine required for daily use.
- **Same** lanes, contracts (`verify:spark`), reviews, keepers.
- Knobs = tune levels/decay in SparkLE; macro workflow is enough for elite loops.

Use [SOP-013](../sops/SOP-013-spark-le-phase-1-feed-spark.md) and example manifests under [examples/](examples/).

---

## Mode B — SparkLE as MIDI controller (Windows)

When you want the **PC sound engine** (phonk_box, future JUCE app) but **SparkLE hands**:

1. Connect SparkLE **USB** to Windows.
2. In **Arturia MIDI Control Center** (or SparkLE controller mode / user MIDI template): ensure **pads send MIDI notes** on channel 10 (or omni).
3. Map pad notes to [phonk-box protocol/MIDI-MAP.md](../../../disklordz/phonk-box/protocol/MIDI-MAP.md) — adjust `phonk_engine.py` `PAD_NOTES` if Arturia defaults differ (often **36–43** or **40–47**; verify with MIDI monitor).
4. Run:

```powershell
cd disklordz\phonk-box\engine
python phonk_engine.py --kit C:\path\to\OneShots --midi Spark
```

Use **MIDI-OX**, **loopMIDI**, or Windows MIDI settings to confirm note numbers before relying on muscle memory.

**Knobs:** map to CC 70–73 (kick pitch, decay, snare snap, hat bright) per MIDI map.

**Caveat:** SparkLE is optimized for **its own samples** in standalone. MIDI mode does **not** automatically load DiskLordz kits — the **PC engine** loads WAVs. You maintain **one kit path** on disk.

---

## Mode C — DIY controller

See [phonk-box](../../../disklordz/phonk-box/README.md). Justified when:

- SparkLE is tied up elsewhere / you want a dedicated “phonk only” desk unit.
- You need **16 hardware step buttons** SparkLE does not provide.
- You are learning embedded MIDI for other projects.

Otherwise: **$8–15 enclosure + $45–70 electronics** is optional spend.

---

## Comparison

| | SparkLE standalone | SparkLE MIDI + PC | DIY + PC |
|--|-------------------|-------------------|----------|
| **Extra hardware cost** | $0 (owned) | $0 | ~$55–85 |
| **Pad quality** | Velocity, sized for fingers | Same | Arcade, no velocity* |
| **Sound** | On-device | PC WAV engine | PC WAV engine |
| **808 Lab manifests** | Native fit | Kit folder on PC | Kit folder on PC |
| **Elite sprint / keepers** | ✅ Primary path | ✅ if engine matches | Extra workflow |

\*Unless you add FSR pads (over budget).

---

## Product naming (one line)

**DiskLordz 808 Lab** = generate kits + **SparkLE** play/sequence (+ optional PC engine for R&D).

**Phonk Box** = optional DIY **or** nickname for **SparkLE + phonk_engine** when in MIDI mode.

---

## Related

[SESSION-SPRINT-5.md](SESSION-SPRINT-5.md) · [phonk-box/ARCHITECTURE.md](../../../disklordz/phonk-box/ARCHITECTURE.md)
