# DiskLordz — Team Roles & Orchestration

**Do not scale by adding chat agents.** Scale by building **factories** (Sound, Plugin, Content, Compiler, QA) and dispatching **team roles** from the **Product Orchestrator**.

**TRON Studio** = autonomous infrastructure. **DiskLordz Audio Lab** = this vertical.

## Invoke

| Role | Skill |
|------|--------|
| **Product Orchestrator** | `disklordz-product-orchestrator` |
| **Audio PM** | `disklordz-audio-pm` |
| **Workflow Automation Engineer** (Taskmaster) | `disklordz-workflow-automation-engineer` |
| **Marketing Growth** | `disklordz-marketing-growth` |

**THOR** (Executive Assistant) lives in the Master Vault Slack `#executive-assistant`; see [`agents/executive-bridge/THOR_TASKMASTER_BRIDGE.md`](agents/executive-bridge/THOR_TASKMASTER_BRIDGE.md). **Taskhelper** = Audio PM daily WO reconciliation.

Specialist work runs as **Work Orders** with `assigned_team` + `assigned_role` — often a Cloud Agent job with the relevant doc path, not a permanent 25th skill.

## Hierarchy

```text
                 Creative Director (human)
                            │
                            ▼
                 PRODUCT ORCHESTRATOR
                            │
        ┌───────────────────┼───────────────────┐
        ▼                   ▼                   ▼
   PRODUCT TEAM        ENGINEERING TEAM     SOUND TEAM
        │                   │                   │
 Product Designer       Architect          Sound Designer
 Market Intelligence    C++ / Kernel       DSP / Character Lab
 UX Designer            Build Engineer     Sample Editor
 Copy / Content         QA Engineer        Kit Designer
        │                   │                   │
        └───────────────────┼───────────────────┘
                            ▼
                      RELEASE TEAM
                   (Compiler / Packaging / Store v1)
                            │
              Audio PM ◄────┴────► Automation Engineer
                            │
                        Airtable
```

## Team roles → default artifacts

| Team | Role | Primary subsystem |
|------|------|-------------------|
| Product | Product Designer | Sonic **problem** spec, family definition |
| Product | Market Intelligence | `intelligence/`, Market Signals table |
| Product | UX Designer | Plugin UI spec, design system |
| Product | Copy / Content | `content-factory/` outputs |
| Sound | Sound Designer | Sound Factory sources |
| Sound | DSP / Sampling | Character Lab modules |
| Sound | Sample Editor | DNA ingest, QA sound profile |
| Sound | Kit Designer | Kit layout, compiler `drum_kit_zip` |
| Engineering | Architect | `plugin-kernel/`, generator |
| Engineering | C++ Engineer | Product modules, DSP wiring |
| Engineering | Build Engineer | CMake, CI, installers |
| Engineering | QA Engineer | `qa-lab/` profiles, DAW matrix |
| Release | Packaging | `product-compiler/` |
| Release | Store v1 | Simple delivery + license PDF |

## Orchestrator responsibilities

1. Accept brief → define **Product Family** + compile manifest targets.  
2. Queue Work Orders by team (see `automation/templates/mvp-work-orders.md`).  
3. Enforce **provenance gate** before Release team runs.  
4. Trigger Content Factory + Demo Generator after QA pass.  
5. Hand approval packet to Creative Director; never set `Released`.

## Audio PM responsibilities

Airtable system of record, weekly cadence, reconcile GitHub ↔ WOs, escalate blocks. Does not replace Orchestrator on **what to compile**.

## Automation Engineer responsibilities

Autonomous Sprint A→D; compiler triggers; feedback ingestion; idempotent workflows.

## Delegation rules

1. Every task → Work Order with `family_id` when applicable.  
2. No SKU ships without Sound DNA + `commercial_ok`.  
3. Plugins ship only through **Plugin Kernel** (no orphan processors).  
4. Version matrix on every Release row.  
5. Human gate: Creative Director → `Released`.

## Repo map

| Path | Subsystem |
|------|-----------|
| `disklordz/sound-dna/` | Asset intelligence |
| `disklordz/sound-factory/` | Generation + Character Lab |
| `disklordz/plugin-kernel/` | Shared JUCE engine + generator |
| `disklordz/preset-factory/` | (planned) |
| `disklordz/product-compiler/` | Manifest compile |
| `disklordz/content-factory/` | (planned) |
| `disklordz/qa-lab/` | (planned profiles) |
| `disklordz/intelligence/` | Signals + graph |
| `MyFirstPlugin/` | Legacy → migrate into kernel |

## Deprecated mental model

The old “12 named agents” list is **absorbed into teams**. Do not create a new Cursor skill per agent unless a role needs a long, stable playbook (Orchestrator, PM, Automation Engineer).
