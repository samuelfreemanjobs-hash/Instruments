# Definition of Done — Release gates

Nothing reaches **Released** without passing all applicable gates. Status tracked on Airtable `Releases` and Product row.

## Gate pipeline

```text
SOURCE → BUILD → UNIT → AUDIO → PLUGIN → INSTALL → PACKAGE → DOC → RC → HUMAN → PUBLISH
```

| Gate | ID | Pass criteria | Owner |
|------|-----|---------------|--------|
| Provenance | G-PROV | All assets `commercial_ok`, SHA registered, license known | Compiler + script |
| Sonic | G-SONIC | Score ≥ threshold per `sonic-scoring.md` | Sound + CD on flagship |
| Genome | G-GENOME | Valid `product-genome` YAML/JSON committed | Product team |
| Build | G-BUILD | CI green on target branch | Build engineer |
| Unit | G-UNIT | Unit tests pass (when present) | Engineering |
| Audio | G-AUDIO | Golden Ear / regression profile pass | QA Lab |
| Plugin tech | G-PLUGIN | pluginval + profile `plugin_technical` | QA Lab |
| Compatibility | G-DAW | Required DAW matrix cells pass or `waived` documented | QA |
| Package | G-PKG | Compiler outputs + checksums | Release |
| Content | G-CONTENT | Content atomizer outputs for SKU | Media |
| License stack | G-LEGAL | VST3/JUCE notices, EULA, sample license | Release |
| RC | G-RC | All above; changelog; version matrix | Orchestrator packet |
| Human | G-HUMAN | Creative Director approves listen + copy + price | Human |
| Publish | G-PUB | Store live, download works | Human/automation |

## WO definition of done

A Work Order is **Done** only when:

- Acceptance criteria checked  
- Output path / URL recorded  
- Downstream gates unblocked or explicitly handed off  
- No `Blocked` without owner  

## Failure

- **Failed** WO → `knowledge/failures/` entry if systemic  
- **Broken** automation → Automations table  
