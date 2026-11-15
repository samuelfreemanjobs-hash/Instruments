# MPC500 + PC workstation (SparkLE optional)

**Version:** 1.1 · **Status:** **Locked plan** (2026-11-03, America/Detroit)  
**Your active rig:** **MPC500 + PC only** → [MPC500-PC-ONLY.md](MPC500-PC-ONLY.md)  
**Index:** [808-LAB.md](../808-LAB.md) · **808 Lab hybrid:** [808-LAB-HYBRID.md](808-LAB-HYBRID.md)

We are **building around a device**, not picking “the best box out of the box.” **Required:** **Akai MPC500** + **Windows PC**. **Optional:** Arturia SparkLE as extra USB MIDI knobs/pads for PC engines.

---

## Roles (non-overlapping)

| Device | Role | DiskLordz use |
|--------|------|----------------|
| **Akai MPC500** | **Foundation instrument** | Sample chop, pad performance, drum programming, 48-track sequencer, pad banks, classic standalone MPC workflow. **Cannot** host our firmware — stays Akai OS. |
| **Arturia SparkLE** *(optional)* | **Companion control surface** | If owned: USB MIDI into PC for params/FX. **Not required** — use MPC500 MIDI + PC UI. |
| **Windows PC** | **Sound engine & intelligence** | `phonk_engine`, factory/`lane-registry`, future Chop Machine, synths, effects. Same manifests and 8-pad law as SaaS. |

**DIY Phonk Box (Teensy):** **deferred** — MPC500 covers pads and sequencing.

---

## Why MPC500 is the foundation

Documented MPC500 strengths (see Akai reference manual):

- 12 velocity- and pressure-sensitive pads, four pad banks  
- Sample record/playback  
- Sequencer (48 tracks per sequence)  
- MIDI in/out for clock and external gear  

**Limitation:** fixed OS/hardware — new engines live on the **PC**, linked by **MIDI + audio**, not by replacing MPC firmware.

---

## Signal flow

```text
┌──────────────────────────────────────┐
│ Windows PC                            │
│  export:sprint-1-3 · phonk_engine    │
│  chop / synth / screw-tape (future)  │
└───────────────▲──────────────────────┘
                │ MIDI in (notes, clock)     audio out → monitors
                │                            (interface when you add one)
     ┌──────────┴──────────┐
     │ Akai MPC500          │     optional: SparkLE USB → PC (CC/pads)
     │ chop · seq · samples │
     │ MIDI out ────────────┘
     └──────────────────────┘
```

**Critical:** MIDI does **not** carry audio. To hear PC instruments in an MPC-centric session, route **PC audio** into your mixer, MPC inputs, or DAW — document your path once and reuse ([SOP-013](../sops/SOP-013-spark-le-phase-1-feed-spark.md) export proof still applies for Lab reviews).

---

## Build order (first useful instrument → advanced)

| Step | Deliverable | Hardware |
|------|-------------|----------|
| **1** | Memphis **sample-chopping app** on PC ([memphis-architect](../../../disklordz/memphis-architect/ARCHITECTURE.md)) | MPC500 triggers; PC UI for chop params |
| **2** | **Custom 808 + cowbell** on PC (`phonk_engine` + lane kits) | MPC MIDI notes → engine; params via PC or optional SparkLE CC |
| **3** | **Soul chord engine** + **screw/tape** FX | PC + MPC mutes; optional SparkLE knobs |
| **4** | **MIDI clock** — MPC500 master or slave vs PC/DAW | Verify stable tempo before long screw sets |
| **5** | Manifest import (`spark-le-manifest.json` → kit paths) + keeper sync | Same 808 Lab folders for MPC samples and PC engine |

Engineering backlog for step 5: [phonk-box/ARCHITECTURE.md](../../../disklordz/phonk-box/ARCHITECTURE.md).

---

## 808 Lab content (unchanged)

One catalog, one CI gate:

```text
Brief → lane-resolve → factory WAV → verify:spark → manifest
  → load samples on **MPC500** (primary)
  → MPC MIDI → PC engines when hybrid playback needed
  → session review → keepers → registry/RAG
```

**8-pad manifest law** ([SOP-012](../sops/SOP-012-spark-le-elite-system-charter.md)) stays the **export contract** for DiskLordz kits. MPC500 may use **pads 1–12** and banks; map **pads 1–8** to manifest roles first; overflow in manifest `overflow[]`.

---

## SparkLE: two valid uses

| Use | When |
|-----|------|
| **Standalone** | Sprint sessions, travel, quick 808 Lab loops without booting PC stack |
| **USB MIDI controller** | **Default when PC engines run** — parameter hands for chop/synth/FX |

See [SPARKLE-MIDI-AND-CONTROLLER.md](SPARKLE-MIDI-AND-CONTROLLER.md).

---

## MPC500 ↔ DiskLordz checklist (operator)

1. Export **OneShots/** + manifest from 808 Lab (factory or [sprint-kits](sprint-kits/)).  
2. Import into MPC500 sample pool; assign **pads 1–8** to manifest roles.  
3. Set sequence BPM from manifest; program drums on MPC sequencer.  
4. PC (optional): `phonk_engine` with same kit; **MPC MIDI out → PC MIDI in**.  
5. Clock: one master (usually **MPC** for live).  
6. Audio: PC speakers/interface; MPC mix for sample-only sessions.  
7. Review: note **MPC500 + PC** in template ([MPC500-PC-ONLY.md](MPC500-PC-ONLY.md)).

---

## Related

[phonk-box/protocol/MIDI-MAP.md](../../../disklordz/phonk-box/protocol/MIDI-MAP.md) · [SESSION-SPRINT-5.md](SESSION-SPRINT-5.md) · [SPARKLE-MIDI-AND-CONTROLLER.md](SPARKLE-MIDI-AND-CONTROLLER.md)
