---
name: mpc-sample-sd-autorun
description: Plan and document MPC Sample microSD autorun deployments — Melodee groovebox, SampQOL UX mod, mpc-sample-sdk packs. One autorun per card; firmware 1.3.0.12 baseline.
---

# MPC Sample — SD `autorun/`

## Rule

**One** top-level `autorun/` on exFAT microSD. Pick **one** stack per card.

| Stack | Repo | Runtime |
|-------|------|---------|
| Stock + community mod | [SampQOL-OS](https://github.com/snarebes/SampQOL-OS) | Closed binary; stock app + UX |
| Replacement app | [melodee-sample](https://github.com/keremimo/melodee-sample) | AGPL JUCE on **sample-base** |
| Rust mod pack | **mpc-sample-sdk** `--pack` | Hooks stock app via autorun |
| MPCTK output | Copy `.xpj` + data to card paths | No autorun required for projects |

## Firmware

**Akai MPC Sample 1.3.0.12** (`1.3.0.12` build) for Melodee, SampQOL, and sdk fingerprint.

## Melodee install (summary)

1. Release zip → copy `autorun/` to card root
2. Back up existing `autorun` if present
3. Boot: stock splash, then Melodee; exit returns to stock app
4. Plugins under `Melodee/Plugins` (aarch64-linux VST3, LV2, CLAP, Open MPC)

**License:** AGPL-3.0 — study patterns; do not fork into closed Disklordz VSTs without review.

## SampQOL

Download only from official releases; replaces `autorun` folder; `mpcqol.conf` on card root.

## Transfer (stock ops)

See [MPC-Sample-Secrets](https://github.com/c1t1zen1/MPC-Sample-Secrets) setup → Sample & project transfer.

## Do not

- Stack Melodee + SampQOL + custom autorun without renaming/moving folders
- Ship AGPL Melodee binaries from this monorepo

## Verify

- User device boot + version screen `[executed]`
- Doc-only planning: tag `[assumed]`