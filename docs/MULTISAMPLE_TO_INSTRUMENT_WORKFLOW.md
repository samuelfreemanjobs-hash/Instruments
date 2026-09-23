# Multisample → new presets / instruments

You capture **static snapshots** from Gearmulator (or factory) VSTi, then **author new products** that are legally and creatively yours as **sample maps + synthesis wrappers** — not shipped firmware LLE.

## Pipeline overview

```text
1. PLAN     capture-plan.tsv (what to render)
2. CAPTURE  DawDreamer / Docker / Vst3OfflineRender → WAV + manifest.tsv + **instrument.map.json**
3. MAP      Edit map zones / SFZ export / HISE sample map (+ velocity layers)
4. PRESET   Macros, filters, MPC programs — your sound design
5. SHIP     HISE VST3 (Track D), mpc-agent kit, or JUCE rompler SKU (Track C)
```

Outputs stay under `gearmulator-lane/multisamples/out/` (**gitignored**). Commit **plans and docs**, not WAVs or ROMs.

---

## Step 1 — Capture plan (one row = one future preset/layer)

Copy [capture-plan.example.tsv](../gearmulator-lane/multisamples/capture-plan.example.tsv) and edit paths:

| Column | Meaning |
|--------|---------|
| `session_name` | Folder under `out/` |
| `instrument_label` | Your **new** instrument/preset name |
| `source_plugin` | Human note (e.g. JE-8086, Osirus) |
| `plugin_path` | `.vst3` bundle path |
| `program` | Host program index |
| `preset_path` | Optional `.fxp` / `.vstpreset` |
| `note_*`, `velocity`, `seconds` | Grid |

Run the plan:

```bash
pip install -r gearmulator-lane/requirements-dawdreamer.txt
python3 scripts/gearmulator/render_capture_plan.py --plan gearmulator-lane/multisamples/my-plan.tsv
```

Single session without a plan file:

```bash
python3 scripts/gearmulator/render_multisample_dawdreamer.py \
  --plugin /path/to/JE-8086.vst3 \
  --instrument-label MyNewRompler_Lead \
  --name je8086_lead_v100 --program 0 \
  --note-start 48 --note-end 72 --note-step 1 --velocity 100
```

---

## Step 2 — Build a playable map (start of your “instrument”)

Every capture session writes **`instrument.map.json`** automatically ([MULTISAMPLE_MAP_FORMAT.md](MULTISAMPLE_MAP_FORMAT.md)). Re-run finalize if you hand-edit `manifest.tsv`:

```bash
python3 scripts/gearmulator/finalize_multisample_session.py \
  gearmulator-lane/multisamples/out/je8086_lead_v100_p0_v100 --sfz
```

Or export SFZ only from the canonical map:

```bash
python3 scripts/gearmulator/map_to_sfz.py \
  gearmulator-lane/multisamples/out/je8086_lead_v100_p0_v100/instrument.map.json
```

Import SFZ + WAV folder into:

- **HISE** (Track D) — fastest sellable rompler; see [HISE_SKETCH_LANE.md](HISE_SKETCH_LANE.md)
- **MPC Software** — via **mpc-agent** (programs, keygroups, chop points)
- **JUCE** — new sampler SKU when Planner opens a factory WO

Refine in the sampler: loop points, crossfade zones, RR, filter/EQ — that work defines **your preset**.

---

## Step 3 — Product types you can create

| Product | Captures | Authoring | Track |
|---------|----------|-----------|-------|
| **Rompler VST** | 1+ sessions / velocity layers | HISE macros + UI | D → optional C port |
| **MPC expansion** | Kits per genre / machine | mpc-agent | mpc-agent repo |
| **Hybrid instrument** | Multisample core + wavetable (Wave909-style) | JUCE | C + spec |
| **Preset pack only** | Same map, multiple macro presets | HISE/MPC | D / mpc |
| **Reference library** | No retail | Internal tuning vs JD Upgraded | E |

Each **retail SKU** needs its own Airtable `product_id` and Marketing sign-off ([DISKLORDZ_PLUGIN_TRACKS.md](DISKLORDZ_PLUGIN_TRACKS.md)).

---

## Step 4 — Naming convention (recommended)

```text
out/<session_name>/instrument.map.json   ← canonical for mpc-agent / HISE / SFZ
out/<session_name>/manifest.tsv          ← render log
instrument_label: Brand_Machine_Character_VelLayer
e.g. Disklordz_JP8000_SawLead_v100 + Disklordz_JP8000_SawLead_v127 → one instrument, two velocity layers
```

Merge velocity layers in HISE/MPC; keep separate sessions in the plan file for clarity.

---

## Legal & repo policy

- **ROM / firmware:** your hardware; never commit to git.
- **WAV captures:** gitignored; retail = your compliance review (derivative of emulated firmware).
- **Do not** embed captures in JD Upgraded `jdupg_cleanroom.rom` ([INSTALLER_POLICY.md](INSTALLER_POLICY.md)).
- **Gearmulator source:** GPLv3 — keep separate from proprietary `Source/`.

---

## Related tools

- [MPC_AGENT_DAWDREAMER.md](MPC_AGENT_DAWDREAMER.md) — DawDreamer vs C++ host  
- [MPC_AGENT_DAWDREAMER.md](MPC_AGENT_DAWDREAMER.md) + submodule `mpc-agent/` — MPC export (growing)  
- Gearmulator Track E (PR #54) — Docker, fork, JE-8086 build  
- [JUCE_FACTORY_AGENT.md](JUCE_FACTORY_AGENT.md) — ship native JUCE rompler after sketch validates  
