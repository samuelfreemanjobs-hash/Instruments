# Asset graph

Edges stored in Airtable **Asset Relations** (and optionally exported JSON).

## Node

Sound DNA record (`id`) + `sha256` for dedup.

## Edge types

| Type | Meaning |
|------|---------|
| `derived_from` | Variant ← parent |
| `similar_to` | Neighbor for kit building |
| `used_in` | Asset → product id |
| `processed_with` | Asset → Character Lab module |
| `compatible_with` | Asset → preset id |
| `belongs_to` | Asset → family_id |

## Queries (business value)

- Unused approved assets with score ≥ 70  
- Assets used in &lt; 2 products (underutilized)  
- Minimal-work new SKU: family + unused DNA + existing kernel preset  

## Example

```text
DL-KICK-0047
   ├── derived_from: (synthesized)
   ├── processed_with: sp-resample, 12-bit
   ├── used_in: DL-KIT-DDD-001, DL-PLUGIN-DRUM-01
   └── belongs_to: DL-FAMILY-DIGITAL-SAMPLER
```
