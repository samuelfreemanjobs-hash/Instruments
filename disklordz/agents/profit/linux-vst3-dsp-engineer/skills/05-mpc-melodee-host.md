# Skill 05 — MPC Sample & Melodee host

## Two hosts (do not conflate)

| Runtime | VST3 aarch64? | Plugin picker |
|---------|----------------|---------------|
| **Melodee** (SD `autorun`) | Yes — primary target for community VST3 | In-app browser; `Melodee/Plugins/` |
| **Stock MPC Sample 1.3.0.12** | aarch64 for some paths; **no** general VST3 browser like Melodee | Inserts / Open MPC workflows |

Melodee requires firmware **1.3.0.12** on MPC Sample.

## Accepted plugin formats (Melodee) [inspected]

From [melodee-sample README](https://github.com/keremimo/melodee-sample):

- VST3 with `Contents/aarch64-linux/`
- LV2 / CLAP aarch64
- Open MPC plugin folders
- VST2 `.so` (aarch64 or armv7 bridge)

Windows/macOS-only builds are shown dimmed in picker.

User install steps, official Melodee synths, and third-party examples: [docs/MPC_SAMPLE_VST3.md](../../../../docs/MPC_SAMPLE_VST3.md) (“Install on MPC Sample” onward).

## CPU / UI budget [inspected]

From Open MPC `docs/MPC_SAMPLE.md`:

- **4× A55** — not Live III big cores; ~3× perf gap vs A76 in their notes
- **128-sample** blocks @ 44.1 kHz
- **320×240** display; CPU repaints — minimize editor refresh rate and complexity
- Example observation: heavy FX ~25% of one core each (not a hard limit)

## Reference plugins

- [mpc-vst-melodee-chip/poly/pd](https://github.com/keremimo) — release `*-mpc-sample.zip` layout
- Melodee built-in engines extracted to standalone Open MPC ports

## Research clones

```bash
git clone --recurse-submodules https://github.com/keremimo/melodee-sample.git
git clone https://github.com/keremimo/sample-base.git
git clone --recursive https://github.com/open-mpc/mpc-vst-plugins.git
```