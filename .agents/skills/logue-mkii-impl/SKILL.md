---
name: logue-mkii-impl
description: Implement mkII DSP in osc.h (/logue-mkii-impl).
---

# Implement (`/logue-mkii-impl [slug]`)

Read `.agent/design.md`, `PORTING.md`, and reference `src/mkii/oscillators/tr808_kick_phonk/osc.h`.

## Rules

- Float samples in `process()`; clamp output
- `setParameter(uint8_t index, uint16_t value)` — value **0–1023**
- `num_param` ≤ 10
- No malloc in audio path
- Mark completion: `/* PORT_COMPLETE */` at top of `osc.h`

## v1 port

Translate logic from v1 `OSC_CYCLE` source listed in `PORTING.md`; do not assume auto-port.

Update `status.json`: `current_phase`: `impl`.
