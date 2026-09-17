# Roland TR-808 Rhythm Composer (PWA)

**Virtual analog emulation** — single-file Progressive Web Application (`index.html`).

**Product origin (Sep 16, 2026):** Initial complete emulation brief (personal session) documented below and implemented in this package for Disklordz.

## Circuit modeling (web engine)

| Subsystem | Voices | Model |
|-----------|--------|--------|
| **Bridged-T resonator** | BD, SD body, LT/MT/HT, LC/MC/HC, RS | Shock-excited resonant decay; pitch/envelope exponentials |
| **6-oscillator Schmitt cluster** | CY, CH, OH | Inharmonic square cluster; **CH/OH choke** |
| **Shaped noise & bursts** | CP, SD snappy, MA | Multi-burst clap; filtered noise |
| **Twin-T ring** | CL, CB | High-Q ringing partials |
| **Analog saturation** | Master | Asymmetric diode/transistor curve; drive + output soft clip |

Web implementation is **real-time VA** (not SPICE/WDF). The **JUCE plugin** track (`vst-tr808/`) targets higher-fidelity circuit emulation.

## User interface

- **Horizontal modern vintage faceplate**
- **16-step** sequencer with classic **color groups** (Red / Orange / Yellow / White)
- **Rotary dials** with drag physics (levels, tone, tempo, swing, drive)
- **Tap tempo**, **swing**, **per-step accent** matrix
- **16 velocity pads** + keyboard map
- **Mobile landscape** layout

## MPC Sample (Akai)

Portable **MPC Sample** sampler — not MPC Beats (desktop). See [docs/MPC_SAMPLE_WORKFLOW.md](docs/MPC_SAMPLE_WORKFLOW.md).

## License

Disklordz / Instruments monorepo — follow repository license.
