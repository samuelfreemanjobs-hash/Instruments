# Phonk Box firmware (USB MIDI only)

**No audio on the MCU.** Firmware reads buttons/encoder and sends USB MIDI.

## Recommended targets

| Board | Why |
|-------|-----|
| **Teensy 4.0** | Mature USB MIDI examples, low latency |
| **RP2040** (Adafruit KB2040 / Pico) | Cheaper; `tinyusb` MIDI device |

## Phase 1 minimum firmware

1. 8 pad switches → Note On/Off (see [../protocol/MIDI-MAP.md](../protocol/MIDI-MAP.md)).
2. Debounce ~5–10 ms.
3. Play / Stop notes 120 / 121.
4. One encoder → CC 1 (BPM) or CC 70 (kick pitch) with mode button.
5. USB descriptor name: **`DiskLordz Phonk Box`** (match `--midi "DiskLordz Phonk Box"`).

## Sketch layout (placeholder)

```text
firmware/
  teensy41/PhonkBoxMidi/   # Arduino + Teensyduino (add when hardware arrives)
  rp2040/                  # optional second port
```

Until sketches land, use **virtual MIDI** (loopMIDI on Windows) + a second keyboard or [sendmidi](https://github.com/gbevin/SendMIDI) for engine dev.

## Safety

- 3.3 V logic; arcade buttons to GND with `INPUT_PULLUP`.
- No mains; USB bus power only.
