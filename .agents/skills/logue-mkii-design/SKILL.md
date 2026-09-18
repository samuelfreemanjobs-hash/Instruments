---
name: logue-mkii-design
description: DSP algorithm and knob mapping design (/logue-mkii-design).
---

# Design (`/logue-mkii-design [slug]`)

Read `.agent/brief.md` and `docs/nts1-mkii-porting.md`.

## Write `.agent/design.md`

- Pseudocode for `Osc::process()`
- Anti-aliasing strategy (band-limited tables vs accept grit)
- Use of host pitch / filter params if any
- Per-knob: index, name, 0–1023 perceptual curve (log/sigmoid/linear)
- Internal LFO vs `shape_lfo` if needed
- Compare complexity to golden `tr808_kick_phonk` when helpful

## Update status

`current_phase`: `design`, append to `phases_completed`.

No full `osc.h` yet unless user asked to revise an existing port.
