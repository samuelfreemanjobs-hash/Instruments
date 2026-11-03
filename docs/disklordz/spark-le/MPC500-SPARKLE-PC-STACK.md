# MPC500 + SparkLE + PC stack (DiskLordz workstation)

**Version:** 1.0 · **Status:** **Locked plan** (2026-11-03, America/Detroit)  
**Index:** [808-LAB.md](../808-LAB.md) · **808 Lab hybrid:** [808-LAB-HYBRID.md](808-LAB-HYBRID.md)

We are **building around a device**, not picking “the best box out of the box.” You own **Akai MPC500** and **Arturia SparkLE**; custom DiskLordz software runs on **Windows**.

---

## Roles (non-overlapping)

| Device | Role | DiskLordz use |
|--------|------|----------------|
| **Akai MPC500** | **Foundation instrument** | Sample chop, pad performance, drum programming, 48-track sequencer, pad banks, classic standalone MPC workflow. **Cannot** host our firmware — stays Akai OS. |
| **Arturia SparkLE** | **Companion control surface** | USB MIDI into PC: phonk drum engine params, future chop UI macros, 808/cowbell synth, soul chord engine, screw/tape FX. Custom maps via Arturia MIDI Control Center. Optional: still load 808 Lab kits **standalone** for portable sprint sessions. |
| **Windows PC** | **Sound engine & intelligence** | `phonk_engine`, factory/`lane-registry`, future Chop Machine, synths, effects. Same manifests and 8-pad law as SaaS. |

**DIY Phonk Box (Teensy):** still **deferred** — MPC500 + SparkLE already cover pads and sequencing.

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
                    ┌─────────────────────────────────┐
                    │ Windows PC                       │
                    │  phonk_engine / Chop / synths   │
                    │  screw-tape FX / soul chords     │
                    └────────▲───────────────▲────────┘
                             │ USB MIDI      │ audio out
                             │ (notes, CC,   │ (interface →
                             │  clock)       │  mixer / MPC in)
              ┌──────────────┴───┐       ┌───┴──────────────┐
              │ Arturia SparkLE   │       │ Audio interface   │
              │ knobs + 8 pads    │       │ (plan explicitly) │
              │ → PC parameters   │       └───────────────────┘
              └───────────────────┘
                             │
                    MIDI clock / notes
                             │
              ┌──────────────▼──────────────┐
              │ Akai MPC500                  │
              │ chop · sequence · sample play  │
              │ MIDI out → PC (optional)       │
              └───────────────────────────────┘
```

**Critical:** MIDI does **not** carry audio. To hear PC instruments in an MPC-centric session, route **PC audio** into your mixer, MPC inputs, or DAW — document your path once and reuse ([SOP-013](../sops/SOP-013-spark-le-phase-1-feed-spark.md) export proof still applies for Lab reviews).

---

## Build order (first useful instrument → advanced)

| Step | Deliverable | Hardware |
|------|-------------|----------|
| **1** | Memphis **sample-chopping app** on PC (start from [memphis-architect](../../../disklordz/memphis-architect/ARCHITECTURE.md) or dedicated Chop Machine) | MPC500 triggers chops; SparkLE maps chop length / start / filter |
| **2** | **Custom 808 + cowbell** synth on PC (`phonk_engine` + lane kits) | SparkLE CC 70–73; MPC sequences MIDI notes to engine |
| **3** | **Soul chord engine** + **screw/tape** pitch-drop FX | SparkLE knobs; MPC mutes/sends via MIDI or audio mix |
| **4** | **MIDI clock** — MPC500 master or slave vs PC/DAW | Verify stable tempo before long screw sets |
| **5** | Manifest import (`spark-le-manifest.json` → kit paths) + keeper sync | Same 808 Lab folders for MPC samples and PC engine |

Engineering backlog for step 5: [phonk-box/ARCHITECTURE.md](../../../disklordz/phonk-box/ARCHITECTURE.md).

---

## 808 Lab content (unchanged)

One catalog, one CI gate:

```text
Brief → lane-resolve → factory WAV → verify:spark → manifest
  → load samples on MPC500 (performance) and/or SparkLE (portable)
  → SparkLE MIDI → PC when using custom engines
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
4. PC: start engine with same kit folder; SparkLE on correct MIDI port.  
5. Clock: choose **one master** (usually MPC for live, PC for screw FX sync tests).  
6. Audio: PC out → interface → monitors (and MPC mix if sampling PC output).  
7. File review with `batch_id` + note **MPC** vs **SparkLE standalone** in template.

---

## Related

[phonk-box/protocol/MIDI-MAP.md](../../../disklordz/phonk-box/protocol/MIDI-MAP.md) · [SESSION-SPRINT-5.md](SESSION-SPRINT-5.md) · [SPARKLE-MIDI-AND-CONTROLLER.md](SPARKLE-MIDI-AND-CONTROLLER.md)
