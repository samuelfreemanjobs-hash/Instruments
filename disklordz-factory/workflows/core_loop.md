# Factory operating system — core loop

Stages run per **batch** (e.g. 25 assets), not per ad-hoc song.

| Stage | Owner agent(s) | Output |
|-------|------------------|--------|
| DISCOVER | Market Intelligence | Opportunity briefs |
| IDEATE | A&R, Creative Director | Selected concepts; rejects logged |
| GENERATE | Music Producer, Sound Designer | Briefs, palettes, MIDI/stem plans |
| SELECT | A&R, Orchestrator | Batch slot assignments |
| PRODUCE | Human + DAW/hardware | Stems, arrangements |
| HUMANIZE | Operator (MPC/SP-808 lane) | Resampled, fingerprinted audio |
| QA | Audio / Product / Visual QA, Rights | PASS → queue; FAIL → review |
| PACKAGE | Sample Miner, Product Manager | Kits, presets, bundles |
| PUBLISH | Content Producer, Copywriter | Titles, descriptions, thumbnails |
| DISTRIBUTE | Content Producer | Platform uploads (post-approval) |
| MEASURE | Analytics | KPIs per asset/cluster |
| LEARN | Analytics → Orchestrator | Rules for next batch |

API enum `BatchStage` mirrors this list for dashboard progress bars.
