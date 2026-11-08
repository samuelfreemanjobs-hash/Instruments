# Phonk Box BOM (~$100 USD)

Approximate prices (2026 retail); shop sales vary.

| Qty | Part | Purpose | ~$ |
|-----|------|---------|-----|
| 1 | **Teensy 4.0** or **RP2040** board with USB | USB MIDI device MCU | 20–35 |
| 8 | Arcade buttons 24mm | Drum pads | 12–18 |
| 1 | Rotary encoder + knob | BPM / param | 4–8 |
| 3 | Tact switches | Play / stop / mode | 2 |
| 1 | Protoboard or perf PCB | Wiring | 5–10 |
| — | Wire, resistors (10k pull-ups) | Debounce | 5 |

**5. Enclosure + connectors (defer until stable)** | See [ENCLOSURE-AND-CONNECTORS.md](ENCLOSURE-AND-CONNECTORS.md) | **$8–15** |

| Item | Notes | ~$ |
|------|--------|-----|
| Project box or reused shell | Drill after cardboard mock | 0–8 |
| USB cable / panel pigtail | Only if not using cable you own | 0–5 |
| JST-XH or screw terminals | Button harness, serviceability | 3–7 |

**Phase 1 desk total (no box):** ~**$45–70** + PC.  
**With item 5 enclosed:** add **$8–15** → still under **~$100** for core hardware.  
Add **16 step LEDs** → ~+$15.

**Total (full enclosed, no LEDs):** ~**$55–85** typical.

## Not in BOM (you already have)

- **Windows PC** — sound engine, storage, UI
- **Speakers / headphones**
- **DiskLordz kits** — WAV one-shots from factory / SparkLE

## Upgrade path (over budget later)

- 16 step LEDs + shift register
- Second encoder (swing / decay)
- Proper pad sensors (FSR) instead of arcade clicks
