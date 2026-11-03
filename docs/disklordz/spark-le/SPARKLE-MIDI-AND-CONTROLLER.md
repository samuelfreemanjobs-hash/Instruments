# SparkLE as controller vs DIY hardware

**Version:** 1.2 · Companion to [MPC500-SPARKLE-PC-STACK.md](MPC500-SPARKLE-PC-STACK.md) and [808-LAB-HYBRID.md](808-LAB-HYBRID.md).

## Decision (locked)

**Workstation stack:** **MPC500** = performance foundation · **SparkLE** = companion MIDI for PC DiskLordz engines · **PC** = chop / phonk / synth / screw-tape. See [MPC500-SPARKLE-PC-STACK.md](MPC500-SPARKLE-PC-STACK.md).

| Date | Decision |
|------|----------|
| 2026-11-03 (America/Detroit) | SparkLE-only controller (808 Lab sprint portable path) |
| 2026-11-03 (America/Detroit) | **MPC500 + SparkLE + PC** — build custom software around MPC; SparkLE controls PC params; no DIY Phonk Box |

**DIY Phonk Box:** deferred — MPC500 covers sequencing/chop; SparkLE covers extra hands.

---

## Short answer

SparkLE is **not** competing with MPC500. It is the **best USB surface** for PC-side engines (knobs, 8 pads, custom Arturia maps). For **offline** sprint kits, SparkLE **standalone** still works without the PC stack.

---

## Four modes

| Mode | What happens | Best for |
|------|----------------|----------|
| **A. SparkLE standalone** | DiskLordz one-shots/loops **into SparkLE**; play and sequence on device | Portable **808 Lab** sprint, travel, quick loops |
| **B. SparkLE USB MIDI → PC** | SparkLE → **phonk_engine**, chop app, synths, FX | **Default with PC stack** — parameter performance |
| **C. MPC500 + PC audio/MIDI** | MPC sequences/samples; PC engines; SparkLE on CC | **Main DiskLordz workstation** |
| **D. DIY Phonk Box** | Teensy → PC | Out of scope unless MPC + SparkLE gap documented |

**Operating plan:** **C** for production build-out; **A** for sprint when MPC/PC not set up; **B** whenever PC engines are running.

---

## Mode A — SparkLE standalone (portable Lab)

```text
DiskLordz factory / manifest → OneShots + Loops → SparkLE user samples → pads 1–8
```

Use [SOP-013](../sops/SOP-013-spark-le-phase-1-feed-spark.md) and [examples/](examples/) / [sprint-kits/](sprint-kits/).

---

## Mode B — SparkLE as PC control surface (Windows)

1. Connect SparkLE **USB** to Windows (separate from MPC500 MIDI ports if both active).
2. **Arturia MIDI Control Center:** pads → notes; knobs → CC 70–73+ per engine.
3. Map to [phonk-box protocol/MIDI-MAP.md](../../../disklordz/phonk-box/protocol/MIDI-MAP.md).
4. Run PC engine, e.g.:

```powershell
cd disklordz\phonk-box\engine
python phonk_engine.py --kit C:\path\to\OneShots --midi Spark
```

**Knobs:** CC 70–73 (kick pitch, decay, snare snap, hat bright) — extend for chop/soul/screw apps.

---

## Mode C — MPC500 foundation

Chop and sequence on MPC; trigger PC instruments via MIDI; hear PC via **audio routing** (not MIDI). Details: [MPC500-SPARKLE-PC-STACK.md](MPC500-SPARKLE-PC-STACK.md).

---

## Mode D — DIY controller

See [phonk-box](../../../disklordz/phonk-box/README.md). Not on roadmap while MPC500 + SparkLE are in use.

---

## Product naming (one line)

**DiskLordz 808 Lab** = one pipeline · **MPC500** performs · **SparkLE** controls PC · **PC** generates intelligence.

**Phonk Box** = PC engine tree (+ optional nickname for SparkLE + `phonk_engine`).

---

## Related

[808-LAB.md](../808-LAB.md) · [SESSION-SPRINT-5.md](SESSION-SPRINT-5.md) · [phonk-box/ARCHITECTURE.md](../../../disklordz/phonk-box/ARCHITECTURE.md)
