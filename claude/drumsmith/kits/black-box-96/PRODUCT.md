# BLACK BOX 96 — The Original Machine

| Field | Value |
|-------|--------|
| **Store title** | **BLACK BOX 96** |
| **Identity** | The Original Machine |
| **Era** | Signature Era |
| **Collection** | DiskLordz Black Box Collection |
| **SKU** | BB96 |
| **Category** | Drum sample kit (oneshots + loops) |
| **Promise** | Signature vintage drum-machine sound — thick 808s, aggressive snares, metallic cowbells, crunchy hats, analog toms, dry claps |
| **Target hits** | 200–250 (locked inventory **240**) |
| **Format** | 44.1 kHz · PCM16 · mono |
| **Street price** | HITL ($24–$34 band) |
| **release_ready** | **false** until HITL |

## Positioning

Part of **DISKLORDZ BLACK BOX — THE LOST DRUM MACHINE COLLECTION**.  
Flagship of four eras. Original synthesis — not a ROM dump.

## Rebuild

```bash
cd claude/drumsmith
python3 scripts/black_box/generate_bb96.py
python3 scripts/black_box/package_bb96.py --zip /opt/cursor/artifacts/Black_Box_96.zip
```
