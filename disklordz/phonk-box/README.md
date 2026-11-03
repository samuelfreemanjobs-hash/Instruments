# DiskLordz Phonk Box

**PC sound engine** for 808 Lab — **MPC500** chops/sequences, **SparkLE** USB MIDI controls params ([MPC500 + SparkLE + PC stack](../docs/disklordz/spark-le/MPC500-SPARKLE-PC-STACK.md)). DIY USB box deferred.

Read **[ARCHITECTURE.md](ARCHITECTURE.md)** first.

## Quick start (no hardware yet)

```bash
cd engine && pip install -r requirements.txt && python phonk_engine.py --self-test
```

## Hardware

[BOM ~$100](docs/BOM-100USD.md) · [Enclosure & connectors](docs/ENCLOSURE-AND-CONNECTORS.md) · [Firmware](firmware/README.md) · [MIDI map](protocol/MIDI-MAP.md)

## Software stack

| Piece | Where |
|-------|--------|
| Sound + sequencer | PC (`engine/`, later JUCE standalone) |
| Controller | USB MIDI (Teensy 4.0 or RP2040) |
| Drum content | 808 Lab / SparkLE 8-pad kits |
