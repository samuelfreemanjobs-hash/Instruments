# Streaming architecture (multi-GB ROM)

Do **not** load entire factory + expansions into RAM.

## Data path

```text
PRESET / PATCH
    ↓
TONE → MULTISAMPLE ID
    ↓
SAMPLE ZONES (SampleAsset ids)
    ↓
SAMPLE CACHE (hot PCM in RAM)
    ↓
DISK STREAMING (cold tails, on-demand)
```

## Policy (design targets)

| Parameter | Typical value | Notes |
|-----------|---------------|-------|
| Preload size | 256–2048 ms | Start of each zone loaded at note-on prep |
| Streaming threshold | Zones > N KB | Stream tail from `.dlrrom` or loose WAV |
| Cache budget | 256–512 MB user configurable | LRU eviction |
| Background load | Message thread | Read-ahead next zone in multisample |
| Expansion mount | ContentEngine | Add/remove card paths without restart (P1) |

## Threading

- **Audio thread:** cache hit only; no disk I/O, no locks that block
- **Background:** open files, fill cache lines, report missing samples to UI

## Missing samples

If entitlement or path fails: silence + log on message thread; validator catches at build time.

## Code (P0 stubs)

- `Source/Content/StreamingSampleCache.h` — interface only
- `Source/Content/ContentEngine.h` — package mount list

P0 rompler still loads full `.dlrrom` cards for small procedural factory; streaming activates when `DL_PKG_FACTORY` exceeds budget in [ROM_BUDGET_V1.md](../../DisklordzSynth/Docs/ROM_BUDGET_V1.md).
