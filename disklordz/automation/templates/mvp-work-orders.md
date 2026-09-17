# MVP work order template (Product Factory slice)

Audio PM: when a Product moves to `Briefed`, create a Project and queue these WOs (adjust titles per product).

| Order | assigned_agent | title | acceptance_criteria (summary) |
|-------|----------------|-------|-------------------------------|
| 1 | audio-product-director | Product spec | Spec doc with user stories, sonic references, out-of-scope |
| 2 | plugin-architect | Plugin architecture | Module list, params, preset format, CPU notes |
| 3 | juce-engineer | Processor scaffold | Builds in CI; APVTS params wired |
| 4 | dsp-engineer | Core DSP | Matches spec on reference material |
| 5 | plugin-ui-engineer | Editor UI | All params automatable; resizable minimum |
| 6 | plugin-qa | Plugin QA | Build, install path, load in host, automation smoke |
| 7 | drum-sound-designer | Raw drum generation | RAW/ folder with named sources |
| 8 | sample-processing | Normalize & catalog | FINAL/ + Sound Assets rows |
| 9 | kit-architect | Kit assembly | Folder layout + PRODUCT_INFO.json |
| 10 | release-agent | Release package | Zip per Release Agent layout |
| 11 | workflow-automation-engineer | A1 GitHub glue | WO → issue automation Live in Automations table |

Dependency: 1 → 2 → (3,4,5 parallel) → 6; (7 → 8 → 9); 10 after 6+9; 11 can start in parallel with 1.
