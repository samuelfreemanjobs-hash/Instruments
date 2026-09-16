# Decision framework

## Who decides

| Decision | Owner |
|----------|--------|
| Sonic taste / constitution override | Creative Director |
| Product family go/no-go | Creative Director + Orchestrator brief |
| Ship to store (`Released`) | Creative Director |
| Pricing in ladder bands | Creative Director |
| Schema / gate changes | Creative Director + documented in `knowledge/decisions/` |
| Implementation detail | Engineering team roles |
| Automation / integrations | Workflow Automation Engineer |
| Airtable truth / WOs | Audio PM |

## Agent rules

- **May** propose; **may not** publish, price, or bypass gates  
- **Must** cite artifact (genome id, WO id, gate name) in handoffs  
- **Escalate** when: provenance unknown, security/production secrets, legal copy, score &lt; threshold on flagship  

## Priority

1. Safety: provenance, licensing, security  
2. Quality: gates, regression, constitution  
3. Reuse: DNA graph, kernel  
4. Speed: only after 1–3  

## Experiments

`r-and-d/experiments/` — no customer ship without promote workflow in `gates/EXPERIMENT_PROMOTE.md`.
