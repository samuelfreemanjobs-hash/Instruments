# DiskLordz — Audio Company OS

Agent-operated VST3 plugin + drum/sample business. This folder is the **control plane**: plans, agent definitions, Airtable schema, and automation assets.

## Start here

| Document | Purpose |
|----------|---------|
| [docs/MASTER_PLAN.md](docs/MASTER_PLAN.md) | End-to-end plan to run the business with minimal human touch |
| [docs/AIRTABLE_OPERATING_MODEL.md](docs/AIRTABLE_OPERATING_MODEL.md) | Tables, statuses, and PM workflows |
| [docs/AUTOMATION_ROADMAP.md](docs/AUTOMATION_ROADMAP.md) | Workflow Automation Engineer backlog and tool choices |
| [AGENTS.md](AGENTS.md) | Team roster and how agents delegate |

## Cursor skills (invoke in chat)

| Skill | Role |
|-------|------|
| `.cursor/skills/disklordz-audio-pm/` | **Audio PM** — products, projects, Airtable, work orders |
| `.cursor/skills/disklordz-workflow-automation-engineer/` | **Automation Engineer** — n8n/Zapier/scripts until the factory runs itself |

## Repos (target layout)

```text
disklordz/
├── plugin-engine/      # JUCE VST3 (this repo: MyFirstPlugin → future split)
├── drum-factory/       # sample generation + processing
├── sample-library/     # sound assets + metadata
├── product-generator/  # kits, zips, store payloads
├── automation/         # workflows + scripts (this folder grows here)
└── website/            # storefront
```

Human role: **Creative Director** — taste, final listen, brand. Everything else trends toward agents + automations.
