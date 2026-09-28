---
name: hermes-elite-presets
description: Hermes presets — factory banks, A&R lane tags, preset file audits. Junova 48 MVP WO-2026-003.
---

# Hermes elite presets

1. Audit: `python3 disklordz/hermes/scripts/hermes_tool.py presets audit --product junova`.
2. Junova MVP: **48** factory presets — bass/pad/poly/lead/FX taxonomy in WO-2026-003.
3. SaaS kits: map `docs/lanes/` + `disklordz/factory/` when on branch.
4. Stable parameter IDs — never rename after ship (`ParameterIDs.h`).
5. Hand implementation to **hermes-dsp** + **hermes-gui** for plugin preset UI.
