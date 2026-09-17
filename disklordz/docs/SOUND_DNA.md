# Sound DNA — Proprietary Asset Intelligence

Random WAV folders do not scale. **Sound DNA** is the atomic metadata + graph layer every factory reads.

## Canonical record

Machine schema: [`../sound-dna/schema/sound-asset.schema.json`](../sound-dna/schema/sound-asset.schema.json)

Example:

```json
{
  "id": "DL-KICK-00421",
  "category": "kick",
  "era": "1990s",
  "character": ["digital", "crushed", "short", "aggressive"],
  "genres": ["trap", "phonk", "memphis"],
  "pitch": "F",
  "duration_ms": 417,
  "sample_rate": 44100,
  "bit_depth": 24,
  "source": "original",
  "processing": ["sampler", "saturation", "compression"],
  "provenance": {
    "license": "original",
    "commercial_ok": true,
    "derivative_of": null,
    "attribution": null
  },
  "variants": ["clean", "saturated", "12-bit", "8-bit"],
  "parent_id": "DL-KICK-00420"
}
```

## Queries the system must answer

- *“Find 20 kicks like early-2000s digital hardware.”* → structured filters + (later) similarity search  
- *“All dark 808s with short decay in Product Family X.”* → knowledge graph + family links  
- **DO NOT SHIP** if `provenance.commercial_ok` is false or `license` unknown  

## Sound generation pipeline

```text
SOUND GENERATOR
      │
      ├── synthesis / sampling / resampling
      ├── pitch / saturation / distortion / compression
      ├── bit reduction / filtering / transient / layering
      ▼
 SOUND VARIANTS (one decision → many assets)
      ▼
 SOUND QA (technical + character checks)
      ▼
 ASSET DATABASE (Airtable + repo/storage + graph edges)
```

One parent kick → clean, saturated, crushed, 12-bit, 8-bit, tape, distorted, resampled variants—each a new DNA row with `parent_id`.

## Hardware Character Lab

**Original algorithms** modeling *classes* of behavior (12-bit, cheap DAC, SP-style resampling, MPC-style workflow)—not unauthorized samples or trademarked product names in marketing.

Character tags link to DSP modules in `sound-factory/character-lab/` and preset templates in Plugin Kernel.

## Knowledge graph edges

Stored in Airtable **Asset Relations** (or graph DB later):

| Relation | Example |
|----------|---------|
| `similar_to` | kick A ↔ kick B |
| `derived_from` | variant → parent |
| `used_in` | asset → product SKU |
| `processed_with` | asset → character preset |
| `compatible_with` | asset → plugin preset id |

Enables Product Intelligence and compiler reuse scoring.

## Storage layout (recommended)

```text
sample-library/
  assets/
    DL-KICK-00421/
      meta.json          # Sound DNA
      DL-KICK-00421.wav
      variants/
        ...
```

PM and factories never rename without updating `id` and paths in sync.
