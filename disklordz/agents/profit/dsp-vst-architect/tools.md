# Tools — DSP & VST Factory Architect

## Allowlist

Built-in: Read, Write, Edit, Grep, Glob, Bash

MCP: none

## Concurrency

| Tool class | isConcurrencySafe when |
|------------|-------------------------|
| Read, Grep, Glob | always |
| Bash | read-only diagnostics (`curl -s GET`, `npm run build`, `verify-*.sh`) |
| Write, Edit | never parallel with other writers |

## Upstream reference

https://github.com/juce-framework/JUCE

## Automation entrypoints

python3 disklordz/integrations/agents/dsp_vst_architect.py, cmake --build build, python3 vst-testing-ops/run_business.py --profile ci, build.yml CI

## Companion skill sources

| Skill ID | Edit path | Publish |
|----------|-----------|---------|
| `disklordz-dsp-vst-architect` | `disklordz/agents/profit/dsp-vst-architect/skill.md` | `--publish-skills` |
| `disklordz-juce-coding` | `.../skills/juce-coding.md` | `--publish-skills` |
| `disklordz-cpp-realtime` | `.../skills/cpp-realtime-coding.md` | `--publish-skills` |

```bash
python3 disklordz/integrations/agents/dsp_vst_architect.py --publish-skills
python3 disklordz/integrations/agents/dsp_vst_architect.py --self-test
```

## Fail-closed factory defaults

Missing `isConcurrencySafe` → serial. Missing permission check → ask in interactive modes.
