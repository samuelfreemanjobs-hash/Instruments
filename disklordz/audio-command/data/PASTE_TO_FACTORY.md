# PASTE_TO_FACTORY (generated)

_Generated 2026-11-08 11:35 UTC · task `plugin-skeleton`_

**Blocker:** none  
**Next owner:** Cursor Factory (separate chat)  
**Liaison:** skill `disklordz-plugin-factory-liaison`

---

DiskLordz Audio — Factory implementer only.

**Implement root:** `DiskLordz-Audio/` (see `DiskLordz-Audio/MILESTONE_001_FACTORY.md`)

**Stage:** B — Blank VST3 shell compiles and loads in DAW  
**Slice:** Delegate: blank VST3 that loads  
**Read:** `docs/DiskLordz_Audio_Master_Roadmap_for_Cursor.md` §18, `docs/DPF_TECHNICAL_ARCHITECTURE.md`, `docs/CHORD_ENGINE_PRD.md`  
**Out of scope:** SaaS website, Stripe, marketing copy

**Acceptance:**
- You have evidence the plugin scans and opens in REAPER or FL.
- Build: cmake artifact path on Windows x64
- Report §18.3: Completed, Files, Build, Tests, Limitations, Next task

**Parallel:** Antigravity may run WO-PREP / WO-CC per `sync-all-antigravity.sh` — do not wait on it for compile.
