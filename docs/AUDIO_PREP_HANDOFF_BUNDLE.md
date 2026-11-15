# Audio prep handoff bundle (PREP-001 / PREP-002)

After Antigravity completes **WO-PREP-001** and **WO-PREP-002**, these paths are the handoff bundle — each lane uses a different piece.

| Path | What it is | Who consumes it | Next use |
|------|------------|-----------------|----------|
| `disklordz/antigravity/outbox-artifacts/chord-engine-ui-tokens.json` | Brand → UI colors, control roles, typography hints | **Cursor Factory** when building CHORD ENGINE UI (Stage G+) | JUCE LookAndFeel or skin config — **not** required for blank VST3 skeleton (Stage B) |
| `DiskLordz-Audio/` | **Product tree** — DPF folders + `MILESTONE_001_FACTORY.md` | **Cursor Factory now** (Stage B) | CMake + VST under `Products/ChordEngine/` |
| `docs/drafts/product-tree/` | Doc mirror + `PRODUCTS.md` | PM / liaison / planning | Private repo split, store copy — **not** compile source of truth |

## Windows VST3 (Stage B proof)

| Method | Command / link |
|--------|----------------|
| **GitHub Actions** | Workflow **DiskLordz Audio (Windows VST3)** on `main` → artifact `chord-engine-vst3-windows-*` |
| **Local** | `powershell -ExecutionPolicy Bypass -File DiskLordz-Audio\build-windows.ps1` after `git pull origin main` |

Details: [DiskLordz-Audio/BUILD.md](../DiskLordz-Audio/BUILD.md) · operator tiers: [CURSOR_OPERATOR_ON_20_PRO.md](CURSOR_OPERATOR_ON_20_PRO.md)

## Order of work

1. **Samuel** — build or download VST3 → REAPER/FL → Coach **`plugin-skeleton`**. Factory chat only if build fails (`PASTE_TO_FACTORY.md`).
2. **UI tokens** — on disk until **first-interface** sprint task.
3. **Drafts** — optional; **`DiskLordz-Audio/`** is authoritative for code.

Inbox clear until next `sync-all-antigravity.sh` dispatch.

## Related

- [AUDIO_PLUGIN_AGENT_LANE.md](AUDIO_PLUGIN_AGENT_LANE.md)
- [CURSOR_FACTORY_LIAISON.md](CURSOR_FACTORY_LIAISON.md)
