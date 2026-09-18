# logue mkII oscillator agent

Cursor agent for **Korg logue SDK v2** custom user oscillators. **NTS-1 mkII** is the default product target.

| File | Role |
|------|------|
| [SYSTEM.md](./SYSTEM.md) | Master system prompt — paste into Project rules or @-mention |
| [docs/logue-mkii-oscillator-agent.md](../../docs/logue-mkii-oscillator-agent.md) | Human workflow + evaluation |
| [.cursor/skills/logue-hardware-port/SKILL.md](../../.cursor/skills/logue-hardware-port/SKILL.md) | Port/build skill |

Invoke: *“Use the logue mkII oscillator agent to …”*

## Slash commands

See [docs/logue-mkii-workflow.md](../../docs/logue-mkii-workflow.md). Quick start:

```
/logue-mkii-setup
/logue-mkii-new my_bass_slug
```

Or stepwise: `/logue-mkii-brief` → `design` → `scaffold` → `impl` → `eval` → `ship`

Golden reference slug: **`tr808_kick_phonk`** — see [docs/logue-mkii-golden-unit.md](../../docs/logue-mkii-golden-unit.md).
