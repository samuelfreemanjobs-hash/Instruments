# Field guide — manual table build

Use **exact** field names (case-sensitive) so scripts and agents match.

## Product Families

| Field | Type |
|-------|------|
| family_id | Single line text (primary) |
| name | Single line text |
| universe_theme | Single line text |
| sonic_problem | Long text |
| status | Single select: Active, Paused, Sunset |

## Products

| Field | Type |
|-------|------|
| product_id | Single line text (primary) |
| name | Single line text |
| division | Single select: Plugin, Sample, Bundle, Expansion |
| status | Single select: Idea, Briefed, Specified, In Development, QA, Ready to Publish, Released, Archived |
| brief | Long text |
| spec_link | URL |
| target_release | Date |
| github_repo | URL |
| price_tier | Single select: free, tier_1, tier_2 |
| ladder_tier | Single select: free, mini, kit, plugin, bundle, collection |
| sonic_problem | Single line text |
| family_id | Single line text |
| compile_targets | Long text |
| product_family | Link → Product Families (optional) |

## Projects

| Field | Type |
|-------|------|
| project_id | Single line text (primary) |
| name | Single line text |
| phase | Single select: 0-Foundation, 1-MVP-Factory-Product, 2-Product-Factory, 3-Full-Roster, 4-Autopilot |
| status | Single select: Planned, Active, Blocked, Done |
| owner_agent | Single line text (or single select) |
| product | Link → Products (optional) |

## Agent Work Orders

| Field | Type |
|-------|------|
| work_order_id | Single line text (primary) |
| title | Single line text |
| assigned_team | Single select: product, sound, engineering, release, meta |
| assigned_role | Single line text |
| assigned_agent | Single select: product-orchestrator, audio-pm, workflow-automation-engineer |
| status | Single select: Queued, In Progress, Review, Done, Failed |
| priority | Single select: P0, P1, P2, P3 |
| github_issue | URL |
| github_pr | URL |
| acceptance_criteria | Long text |
| output_location | Single line text |
| blocked_reason | Long text |
| project | Link → Projects (optional) |

## Sound Assets, Asset Relations, Releases, Market Signals, Customer Feedback, DAW Matrix, Automations

See `base-schema.json` — same naming convention.
