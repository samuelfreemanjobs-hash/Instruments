# Airtable Operating Model — DiskLordz

Audio PM Agent treats Airtable as the **system of record** for products, work, and release state. GitHub holds code; Airtable holds **what** and **why**; artifacts live in repos and storage.

## Base name

`DiskLordz OS` (recommended)

## Tables

See machine-readable field list: [`../airtable/base-schema.json`](../airtable/base-schema.json).

### Products

Sellable units: plugins, kits, bundles, expansions.

| Field | Type | Notes |
|-------|------|-------|
| `product_id` | Single line | e.g. `DL-PLUGIN-DISKCRUSHER` |
| `name` | Single line | Marketing name |
| `division` | Single select | `Plugin` \| `Sample` \| `Bundle` \| `Expansion` |
| `status` | Single select | See lifecycle below |
| `brief` | Long text | Creative Director input |
| `spec_link` | URL | Notion/Google Doc or repo `docs/specs/` |
| `target_release` | Date | Optional |
| `github_repo` | URL | |
| `price_tier` | Single select | `free` \| `tier_1` \| `tier_2` |
| `projects` | Link → Projects | |
| `releases` | Link → Releases | |

**Product status lifecycle**

```text
Idea → Briefed → Specified → In Development → QA → Ready to Publish → Released → Archived
```

Human gate: **`Ready to Publish` → `Released`** (Creative Director).

### Projects

Time-bound execution (one MVP, one plugin version, one kit vol).

| Field | Type | Notes |
|-------|------|-------|
| `project_id` | Single line | `PRJ-2026-001` |
| `name` | Single line | |
| `product` | Link → Products | |
| `phase` | Single select | `0-Foundation` … `4-Autopilot` (maps to MASTER_PLAN) |
| `status` | Single select | `Planned` \| `Active` \| `Blocked` \| `Done` |
| `owner_agent` | Single select | `audio-pm` \| `product-factory` \| `juce-engineer` \| … |
| `work_orders` | Link → Agent Work Orders | |

### Sound Assets

Atomic WAV metadata (library grows forever).

| Field | Type | Notes |
|-------|------|-------|
| `asset_id` | Single line | `DL-KICK-0042` |
| `type` | Single select | kick, snare, clap, hat, perc, 808, fx, vox, … |
| `character` | Multiple select | digital, dirty, short, … |
| `era` | Single select | 1990s, 2000s, … |
| `style` | Multiple select | trap, phonk, memphis, … |
| `file_path` | Single line | Repo or storage URI |
| `sample_rate` | Number | |
| `bit_depth` | Number | |
| `provenance` | Long text | **Required before release** |
| `products` | Link → Products | Kits/plugins using this asset |

### Agent Work Orders

Unit of agent work (replaces ad-hoc chat tasks).

| Field | Type | Notes |
|-------|------|-------|
| `work_order_id` | Single line | `WO-2026-0142` |
| `title` | Single line | |
| `assigned_agent` | Single select | Match skill names |
| `project` | Link → Projects | |
| `status` | Single select | `Queued` \| `In Progress` \| `Review` \| `Done` \| `Failed` |
| `priority` | Single select | P0–P3 |
| `github_issue` | URL | |
| `github_pr` | URL | |
| `acceptance_criteria` | Long text | Testable |
| `output_location` | Single line | Path or artifact URL |
| `blocked_reason` | Long text | |

### Releases

| Field | Type | Notes |
|-------|------|-------|
| `release_id` | Single line | `REL-1.0.0` |
| `product` | Link → Products | |
| `version` | Single line | Semver |
| `status` | Single select | `Draft` \| `Candidate` \| `Shipped` |
| `zip_artifact` | URL | |
| `changelog` | Long text | |

### Automations (registry)

Workflow Automation Engineer documents every integration here.

| Field | Type | Notes |
|-------|------|-------|
| `automation_id` | Single line | |
| `name` | Single line | |
| `platform` | Single select | `Zapier` \| `n8n` \| `Activepieces` \| `Script` \| `GitHub Actions` |
| `trigger` | Long text | |
| `action` | Long text | |
| `status` | Single select | `Planned` \| `Live` \| `Broken` |
| `owner` | Single select | `workflow-automation-engineer` |
| `last_verified` | Date | |

## PM cadence

| Rhythm | Action |
|--------|--------|
| On brief | Create Product + Project; decompose into Work Orders |
| Daily (agent) | Update WO status; link PRs; flag `Blocked` |
| Weekly | Reconcile GitHub ↔ Airtable; stale WOs; report to Creative Director |
| Pre-release | Checklist: QA WO done, provenance on assets, changelog, license file |

## API / MCP access

Preferred order:

1. **Zapier MCP** — `execute_zapier_read_action` / `write` for Airtable (enable Airtable actions in Zapier MCP).
2. **Airtable REST API** — scripts in `disklordz/automation/scripts/` with `AIRTABLE_API_KEY` + `AIRTABLE_BASE_ID`.
3. **Manual** — only for bootstrap; PM should still record state in base.

Audio PM must **never** invent status: always read/write Airtable (or explicitly note sync failure).
