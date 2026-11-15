# Templates — VST Factory / APC / DPCS

Every phase artifact has a template. Fill them with:

```bash
bin/apc init <Name>                                   # plugins/<Name>/ + status.json (from status-template.json)
bin/apc template <Name> dream --class effect --category fx.metering --theme disklordz
bin/apc template <Name> plan|design|verify|release    # one phase at a time (never overwrites; --force to replace)
python3 tools/apc_templates.py --list                 # what each phase creates
```

| Phase | Template | Creates | SOP |
|-------|----------|---------|-----|
| Dream (DPCS 0) | `constitution.template.md` | `DPCS_PHASE0_CONSTITUTION.md` | [SOP-VF-01](../../docs/disklordz/sops/SOP-VF-01-intake-dream.md) |
| Dream (DPCS 1) | `creative-brief.template.md` | `.ideas/creative-brief.md` | SOP-VF-01 |
| Dream (DPCS 1) | `parameter-spec.template.md` | `.ideas/parameter-spec.md` | SOP-VF-01 |
| Plan | `architecture.template.md` | `.ideas/architecture.md` | [SOP-VF-04](../../docs/disklordz/sops/SOP-VF-04-plan.md) |
| Plan | `apc-plan.template.md` | `.ideas/apc-plan.md` | SOP-VF-04 |
| Design | `design-spec.template.md` | `.ideas/design-spec.md` | [SOP-VF-02](../../docs/disklordz/sops/SOP-VF-02-ui-design.md) |
| Verify | `verify-phase1.template.sh` · `verify-phase4.template.sh` | `scripts/verify-phase{1,4}.sh` | [SOP-VF-06](../../docs/disklordz/sops/SOP-VF-06-verify-test.md) |
| Ship | `release-notes.template.md` | `RELEASE_NOTES.md` | [SOP-VF-08](../../docs/disklordz/sops/SOP-VF-08-release-ship.md) |
| State | `status-template.json` | `status.json` (via `bin/apc init`) | [SOP-VF-00](../../docs/disklordz/sops/SOP-VF-00-vst-factory-master.md) |

## Other reusable templates and modules

| What | Where | SOP |
|------|-------|-----|
| Factory-lib DSP block catalog entry | [templates/factory/dsp-block.template.json](../factory/dsp-block.template.json) | [SOP-VF-10](../../docs/disklordz/sops/SOP-VF-10-reusable-module.md) |
| Factory-lib GUI component catalog entry | [templates/factory/gui-block.template.json](../factory/gui-block.template.json) | SOP-VF-10 |
| Factory-lib block C++ contract header | [templates/factory/factory-lib-block.template.h](../factory/factory-lib-block.template.h) | SOP-VF-10 |
| Design-system theme | `python3 vst/factory-lib/design-system/scripts/new_theme.py <id>` (copies `themes/_template`) | [SOP-VF-03](../../docs/disklordz/sops/SOP-VF-03-design-system-theme.md) |
| New VST project | `./tools/new-vst.sh <Name> --type effect\|synth` (copies `vst/template*`) | [SOP-VF-05](../../docs/disklordz/sops/SOP-VF-05-contract-implement.md) |
| Cloud agent prompt for factory work | [docs/disklordz/templates/TEMPLATE-factory-cloud-agent-prompt.md](../../docs/disklordz/templates/TEMPLATE-factory-cloud-agent-prompt.md) | [SOP-VF-11](../../docs/disklordz/sops/SOP-VF-11-agent-delegation.md) |
| Factory intake creative briefs per class | [disklordz/vst-factory/templates/apc/](../../disklordz/vst-factory/templates/apc/) | SOP-VF-07 |

Placeholders: `{{PLUGIN_NAME}}`, `{{PLUGIN_CLASS}}`, `{{CATEGORY}}`, `{{THEME}}`, `{{VERSION}}`, `{{DATE}}`. HTML comments (`<!-- -->`) mark text to replace; `verify-phase1.sh` fails while any remain in the brief. Tests: `python3 -m unittest tools/test_apc_templates.py`.
