# Junova-X, NovaDrum, and TR-808 PWA — three products

These are **not** one SKU, one folder, or one work order. Agents must pick the correct `repoPath` and **dedicated branch** before editing.

**Business stack:** all **native plugins** ship **JUCE** (VST3 + CLAP + Standalone where applicable). iPlug2 folders are **legacy import reference only** (e.g. `vst-juno106/` with Junova-X), not the ship stack.

| Product | Customer-facing role | `repoPath` | Stack | WO prefix |
|---------|------------------------|------------|-------|-----------|
| **Junova-X** | Juno-class **poly synth** | `Junova-X/` (+ legacy `vst-juno106/` import) | **JUCE** VST3 + CLAP | `[Plugin][Junova-X]` |
| **NovaDrum** | TR-808 **circuit-class drum plugin** | `vst-tr808/` | **JUCE** VST3 + CLAP | `[Plugin][NovaDrum]` |
| **TR-808 PWA** | **Browser** drum machine (lead magnet / MPC export) | `disklordz/tr808-pwa/` | Static PWA + Web Audio | **`[TR808-PWA]`** only |

## Branch policy (split — do not use combined handoff)

| Product | Canonical branch | Deprecated |
|---------|------------------|------------|
| **Junova-X** | **`cursor/junova-x-handoff-029a`** | `cursor/junova-tr808-handoff-94ae` (combined; do not open new PRs) |
| **NovaDrum** | **`cursor/novadrum-juce-handoff-029a`** | same combined branch; do not treat `tr808-design-spec-94ae` as second source of truth after novadrum branch lands |
| **TR-808 PWA** | **`cursor/tr808-pwa-app-94ae`** | — (standalone; never mixed with Junova/NovaDrum PRs) |

One PR per product. **`vst-juno106/` stays on the Junova-X branch** as historical iPlug2 reference for porting; it is not NovaDrum and not a fourth SKU.

## What is *not* what

- **NovaDrum** (`vst-tr808/`) ≠ **TR-808 PWA** (`disklordz/tr808-pwa/`).
- **Junova-X** ≠ either drum product.
- **TR-808 PWA** ≠ **Disklordz drum kit SaaS** (`disklordz/website/`). Different lane, WO prefix, and deploy path — see [AGENTIC_OPERATING_MODEL.md](AGENTIC_OPERATING_MODEL.md).
- **Disklordz SaaS** generates sample kits — not NovaDrum engine or PWA shell.

## Where the code lives (`main` vs branches)

| Product | On `main` | Merge from |
|---------|-----------|------------|
| Junova-X | [Junova-X/REPO_HANDOFF.md](../Junova-X/REPO_HANDOFF.md) | `cursor/junova-x-handoff-029a` |
| NovaDrum | [vst-tr808/REPO_HANDOFF.md](../vst-tr808/REPO_HANDOFF.md) | `cursor/novadrum-juce-handoff-029a` |
| TR-808 PWA | [disklordz/tr808-pwa/ARCHITECTURE.md](../disklordz/tr808-pwa/ARCHITECTURE.md) | `cursor/tr808-pwa-app-94ae` |

## Related

- [DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md) — track letters A / B / **P** / G
- [agent-registry.json](agent-registry.json)
