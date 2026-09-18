# DISKLORDZ — A&R Department & Artist Agents

**Head of A&R codename:** `ar-director` · **Display name:** **CROW** (Curators of Raw Output & Waveforms)  
**Role:** Sonic lane integrity, reference sessions, preset DNA, **kit curation sign-off** before Marketing sells  
**Peers:** PM (WOs), FORGE (automation), VARA (story must match sound), Factory (generation)  
**Slack:** `#disklordz-marketing-dept` (tag `CROW:` / `A&R:`) — or create `#disklordz-ar` when volume warrants  
**Instructions:** [disklordz/ar/AGENT_INSTRUCTIONS.md](../disklordz/ar/AGENT_INSTRUCTIONS.md)

---

## Why A&R is in the launch stack

Marketing can **sell**; Factory can **generate**; only A&R + you guarantee **the phonk actually hits**. Tripwire **Memphis Vault Lite** needs:

1. Factory batch (Cursor)  
2. **Lane guardian** filter (artist agent)  
3. **CROW** consistency check vs DL002 / Memphis Vault Vol. 1  
4. **Founder** final legal + taste (automation cannot replace this)

---

## Artist lane agents (OpenClaw / Isaac lineage)

Each **YouTube / factory lane** has a guardian agent — not a persona that posts as a human artist, but a **spec keeper** for prompts, tags, and reference tracks.

| Lane ID | Preset (SaaS) | Artist agent ID | Vibe |
|---------|---------------|-----------------|------|
| DL001 | Boulevard 86 | `artist-boulevard-86` | French touch / filter house |
| DL002 | Midnight Circuit | `artist-midnight-circuit` | Drift / Memphis phonk — **launch hero lane** |
| DL004 | Terminal Mirage | `artist-terminal-mirage` | Cyber funk / MPC clean |
| DL006 | Disklordz Screw | `artist-disklordz-screw` | Screw / slow tape phonk |

Prompts: [`disklordz/ar/subagents/`](../disklordz/ar/subagents/)

**OpenClaw compile sessions** (local): feed **reference WAV paths + session notes** into `disklordz/ar/sessions/` (gitignored samples OK via manifest); guardian agent updates RAG snippets + preset copy in PRs.

---

## CROW subagents

| ID | Role |
|----|------|
| `ar-lane-guardian` | Template for per-lane artists (extend, don’t duplicate) |
| `ar-reference-curator` | Ingest references; output “match this energy” briefs for factory |
| `ar-kit-qc` | Score factory batches; reject muddy/weak hits before pack zip |
| `ar-rag-librarian` | Propose updates to `disklordz/rag/corpus/` from sessions |

---

## Automation (FORGE)

| Event | Action |
|-------|--------|
| Push to `disklordz/ar/**` | `forge-agent-registry.yml` registers agent |
| `[Factory]` kit batch complete | PM WO → `ar-kit-qc` review checklist (future: API hook) |
| Preset copy change | Requires CROW + PM sign-off in WO |

---

## PM partnership

| PM | A&R |
|----|-----|
| Opens `[Factory]` WO for WAV batch | Accepts/rejects kit against lane brief |
| Sets SKU ship date | Confirms sonic acceptance before HELM launch |
| Airtable `product_id` | Links `sonic_lane` field (DL002 etc.) |

---

## Related

- [DISKLORDZ_PHONK_KIT_AI_SOP.md](DISKLORDZ_PHONK_KIT_AI_SOP.md)  
- [DISKLORDZ_SAAS_AGENT_LANES.md](DISKLORDZ_SAAS_AGENT_LANES.md)  
- [disklordz/website/src/lib/presets.ts](../disklordz/website/src/lib/presets.ts)
