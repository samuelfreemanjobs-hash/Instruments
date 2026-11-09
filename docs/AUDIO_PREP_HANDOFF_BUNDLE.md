# Audio prep handoff bundle (PREP-001 / PREP-002)

After Antigravity completes **WO-PREP-001** and **WO-PREP-002**, these paths are the handoff bundle — each lane uses a different piece.

| Path | What it is | Who consumes it | Next use |
|------|------------|-----------------|----------|
| `disklordz/antigravity/outbox-artifacts/chord-engine-ui-tokens.json` | Brand → UI colors, control roles, typography hints | **Cursor Factory** when building CHORD ENGINE UI (Stage G+) | JUCE LookAndFeel or skin config — **not** required for blank VST3 skeleton (Stage B) |
| Private repo `samuelfreemanjobs-hash/DiskLordz-Audio` | **Product tree** — DPF folders + `DOCUMENTATION/milestones/` | **Cursor Factory now** (Stage B) | CMake + VST under `Products/ChordEngine/` |
| `docs/drafts/product-tree/` | Doc mirror + `PRODUCTS.md` | PM / liaison / planning | Private repo split, store copy — **not** compile source of truth |

## Order of work

1. **Factory** — private repo `DiskLordz-Audio` + `disklordz/audio-command/data/PASTE_TO_FACTORY.md` until VST3 loads in a DAW.
2. **UI tokens** — on disk until **first-interface** sprint task.
3. **Drafts** — optional; the private **`DiskLordz-Audio`** repo is authoritative for code.

Inbox clear until next `sync-all-antigravity.sh` dispatch.

## Related

- [AUDIO_PLUGIN_AGENT_LANE.md](AUDIO_PLUGIN_AGENT_LANE.md)
- [CURSOR_FACTORY_LIAISON.md](CURSOR_FACTORY_LIAISON.md)
