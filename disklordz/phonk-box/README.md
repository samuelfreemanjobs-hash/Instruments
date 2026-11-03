# DiskLordz Phonk Box

Custom **phonk drum machine**: programmable sequencer on **Windows**, **USB DIY controller** for pads and params — target **under $100** hardware.

Read **[ARCHITECTURE.md](ARCHITECTURE.md)** first.

## Quick start (no hardware yet)

```bash
cd engine && pip install -r requirements.txt && python phonk_engine.py --self-test
```

## Hardware

[BOM ~$100](docs/BOM-100USD.md) · [Firmware notes](firmware/README.md) · [MIDI map](protocol/MIDI-MAP.md)

## Software stack

| Piece | Where |
|-------|--------|
| Sound + sequencer | PC (`engine/`, later JUCE standalone) |
| Controller | USB MIDI (Teensy 4.0 or RP2040) |
| Drum content | 808 Lab / SparkLE 8-pad kits |
