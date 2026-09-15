# Branding — Resonance

| | |
|--|--|
| **Product** | Resonance |
| **Company** | Instruments |
| **Bundle ID** | `com.instruments.resonance` |
| **Project file** | `*.resonance` (ValueTree XML) |

## Naming in the app

- Window / app title: **Resonance** or **Resonance Hybrid DAW**
- Built-in effects: **Resonance Gain**, **Resonance HPF**, etc.
- Internal plug-in menu: **Resonance internal mix**

## Code

- C++ namespace: `resonance::` (`model`, `view`, `audio`, `controller`, `app`)
- CMake target / binary: `Resonance`

Legacy VMPC2000XL names are removed from user-facing copy and project identifiers. Old `.vmpc` projects are not auto-migrated; re-save under `.resonance` after upgrading.
