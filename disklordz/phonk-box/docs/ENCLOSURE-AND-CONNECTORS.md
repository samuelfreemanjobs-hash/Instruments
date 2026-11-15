# Prototype enclosure & connectors

**Budget line:** **$8–15** (can wait until pad layout and MIDI map are stable).

## Build order

| Step | Buy | When |
|------|-----|------|
| 1–4 | MCU, buttons, encoder, breadboard | First prototype on desk |
| **5** | **Enclosure + connectors + USB** | After firmware + engine feel right |

Skipping the box early saves money and avoids drilling the wrong hole pattern.

---

## Item 5 — what to budget ($8–15)

| Item | Purpose | ~$ |
|------|---------|-----|
| **Project box** (ABS ~100×60×25 mm) or **repurposed** case (old USB hub shell, gamepad donor) | Protect MCU; mount USB | 0–8 |
| **USB cable** (A to micro-B or USB-C, board-dependent) | PC link if not using onboard socket only | 0–5 *(often already owned)* |
| **Connectors** (see below) | Serviceability, strain relief | 3–7 |

**Target:** **$8–15** if you reuse a case and have a cable; **$15** with a new Hammond-style box + JST set.

---

## Connectors (prototype → v1)

### Desk prototype (no enclosure)

- **Dupont / jumper wires** MCU ↔ buttons on breadboard.
- MCU **USB directly to PC** — no panel mount yet.

### First enclosed prototype

| Connection | Recommendation |
|------------|----------------|
| MCU ↔ button harness | **JST-XH 2.54 mm** (2-pin per button) or **Screw terminal block** for quick swaps |
| USB to panel | **Panel-mount USB-B or USB-C extension** (~30 cm pigtail) so you do not stress the Teensy/Pico socket |
| Encoder | Direct solder or **JST 5-pin** (A, B, SW, GND, 3V3 if LED) |
| Strain relief | Zip tie anchor on USB pigtail inside box |

**Skip for v0:** Neutrik, full custom PCB, magnetic pogo pins.

---

## Enclosure options (cheapest → nicer)

1. **Open breadboard** — $0; validate MIDI and layout.
2. **Repurposed plastic** — drill for 8 buttons + USB; sand edges; **best $/durability** for phonk box v0.
3. **Off-the-shelf project box** — e.g. 1590BB-style (drill template after cardboard mock).
4. **3D print** — after dimensions locked in CAD; cost = filament + time.

**Tip:** Cut a **cardboard faceplate** with button spacing first; use it as a drill guide for plastic.

---

## Mechanical checklist (before you drill)

- [ ] Button spacing fits your fingers (8 in 2×4 or 1×8 strip).
- [ ] USB port reaches PC without pulling the MCU off the standoffs.
- [ ] ~20 mm internal height for arcade buttons + nut underneath.
- [ ] Play / stop / mode reachable without hitting pads accidentally.

---

## Related

[BOM-100USD.md](BOM-100USD.md) · [firmware/README.md](../firmware/README.md) · [ARCHITECTURE.md](../ARCHITECTURE.md)
