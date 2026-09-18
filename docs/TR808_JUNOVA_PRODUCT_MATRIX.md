# Junova-X, NovaDrum, and TR-808 PWA — three products

These are **not** one SKU, one folder, or one work order. Agents must pick the correct `repoPath` and branch before editing.

| Product | Customer-facing role | `repoPath` | Stack | Typical WO prefix |
|---------|------------------------|------------|-------|-------------------|
| **Junova-X** | Juno-class **poly synth** (JUCE factory) | `Junova-X/` | JUCE VST3 + CLAP | `[Plugin][Junova-X]` |
| **NovaDrum** | TR-808 **circuit-class drum plugin** (DAW) | `vst-tr808/` | iPlug2 + VST3 (spec today; ship stack per handoff) | `[Plugin][NovaDrum]` |
| **TR-808 PWA** | **Browser** 808-style drum machine (lead magnet / MPC Sample export) | `disklordz/tr808-pwa/` | Static HTML + Web Audio PWA | `[Disklordz][TR808-PWA]` or SaaS WO |

## What is *not* what

- **NovaDrum** (`vst-tr808/`) ≠ **TR-808 PWA** (`disklordz/tr808-pwa/`). Same sonic family; different runtime (native plugin vs browser).
- **Junova-X** (`Junova-X/`) ≠ either drum product. Analog poly synth lane only.
- **Disklordz drum kit SaaS** (`disklordz/website/`, `sound-factory/`) generates **sample kits** — not the NovaDrum engine or the PWA shell.
- **WAVE-909**, **SP-1200**, **Pluggnb Vital pack** are unrelated SKUs; see [agent-registry.json](agent-registry.json).

## Where the code lives today (`main` vs branches)

| Product | On `main` | Landed on branch (merge pending) |
|---------|-----------|----------------------------------|
| Junova-X | Handoff stub only — see [Junova-X/REPO_HANDOFF.md](../Junova-X/REPO_HANDOFF.md) | `cursor/junova-tr808-handoff-94ae` |
| NovaDrum | — | `cursor/junova-tr808-handoff-94ae`, `cursor/tr808-design-spec-94ae` (`vst-tr808/`) |
| TR-808 PWA | Pointer doc only — see [disklordz/tr808-pwa/ARCHITECTURE.md](../disklordz/tr808-pwa/ARCHITECTURE.md) | `cursor/tr808-pwa-app-94ae` |

After merge, update this table and [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md) so paths on `main` match reality.

## Shared semantics (optional, later)

Voice maps and export presets may align between **PWA** and **NovaDrum** for marketing continuity. Implementation stays in **separate trees** until an explicit cross-product WO says otherwise.

## Related

- [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md) — tracks A/B (plugins) vs app lane (PWA)
- [HISE_ANTIGRAVITY_LANE.md](HISE_ANTIGRAVITY_LANE.md) — rompler lane; not Junova/NovaDrum unless port WO
- [agent-registry.json](agent-registry.json) — one active agent per `repoPath`
