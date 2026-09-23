# External logue reference repos (not our product code)

**Policy:** These repositories are **reference and/or personal use only**. They are **not** part of the Instruments mkII collection under `src/mkii/`. The **Korg Logue Agent** must **not** copy their sources into our tree, submodule them, or merge their licenses into our units **unless you explicitly assign a port** (new WO + attribution review).

**Our ship lane:** `src/mkii/oscillators/`, `src/mkii/fx/` — original or v1-ported DSP documented in [logue-oscillator-collection-roadmap.md](logue-oscillator-collection-roadmap.md).

**Optional local clone** (outside git or gitignored): `reference/logue/<repo-name>/` — see [reference/logue/README.md](../reference/logue/README.md).

---

## Catalog (owner-curated)

| Repo | URL | Typical use | mkII / v1.1 notes |
|------|-----|-------------|-------------------|
| **junologue-chorus** | [peterall/junologue-chorus](https://github.com/peterall/junologue-chorus) | Juno-60 **chorus** emulation | Prologue / XD / **NTS-1 mkI** lane — compare to our `juno_dco_osc` on-board chorus; **reference**, not a dependency |
| **cloudlogue-reverb** | [peterall/cloudlogue-reverb](https://github.com/peterall/cloudlogue-reverb) | **Clouds**-style reverb port | v1.1 FX/reverb patterns; contrast with our `dream_reverb_fx` / `cardo_got_wings_spring_revfx` goals |
| **Waves2** | [boochow/Waves2](https://github.com/boochow/Waves2) | **Wavetable** synth (logue SDK) | Explicitly includes **NTS-1 mkII** — WT pipeline / UI ideas for `prophet_vs_wt128`, SQ-80, etc. |
| **logue-fx** | [dukesrg/logue-fx](https://github.com/dukesrg/logue-fx) | Unified **FX** headers across FX types | Scaffold patterns for modfx/revfx/delfx; compare to our `tools/build-mkii-fx.sh` lane |
| **tremoxd** | [ttaschke/tremoxd](https://github.com/ttaschke/tremoxd) | **Tremolo** modfx | Minilogue XD — simple modfx DSP reference |
| **logue-osc** | [dukesrg/logue-osc](https://github.com/dukesrg/logue-osc) | **Oscillator API** extensions, WT osc, web wave inject | **FM48/FM64** + `FM64.sh` DX7 bank inject — primary reference for [minilogue-xd-fm4-dx7 plan](minilogue-xd-fm4-dx7-oscillator-plan.md) |

---

## Agent rules

1. **Read for ideas** (architecture, parameter layout, SDK glue) — cite the repo in design notes, not paste code.
2. **License check** before any literal port (BSD/MIT/GPL varies by repo).
3. **Platform:** Many targets are **v1.1** (`.prg`); our default is **mkII** (`.nts1mkiiunit`). Porting requires [nts1-mkii-porting.md](nts1-mkii-porting.md), not a straight copy.
4. **Do not** add these URLs to weekly idea intake as “new slugs” — they are **existing third-party products**.
5. **RAG:** This file is indexed for memory; cloning upstream repos into RAG is **optional** and must stay under `reference/logue/` (gitignored).

---

## Related internal docs

- [logue-custom-fx-lane.md](logue-custom-fx-lane.md)
- [logue-oscillator-ideas-backlog.md](logue-oscillator-ideas-backlog.md)
- [archive/quarantined-claude-osc-bundle/](archive/quarantined-claude-osc-bundle/) — unrelated bad prompts; still do not use for codegen
