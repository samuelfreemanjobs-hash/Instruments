# Hardware capture rack (maintainer studio)

**Owner:** Sam Freeman — **not in git as audio**, only this inventory for agents and capture planning.

**Purpose:** Cloud Agents and mpc-agent workflows should assume multisamples come from **this hardware** (MIDI + audio interface) or **MPC/Maschine exports**, not from ASR/SU/JP/Virus ROMs unless separately acquired.

**Canonical map format:** [MULTISAMPLE_MAP_FORMAT.md](MULTISAMPLE_MAP_FORMAT.md) · **MPC path:** [MPC_AGENT_DAWDREAMER.md](MPC_AGENT_DAWDREAMER.md)

---

## Synths & keyboards

| Device | Capture notes |
|--------|----------------|
| Arturia **MicroFreak** | Digital; strong plucks/bells/noise for trap layers. MIDI grid → WAV. |
| Korg **MicroKorg** | VA/rompler; MIDI grid per preset. |
| Korg **Micro-X** | MIDI grid; rompler-style zones. |
| Novation **Ultra Nova** | VA supersaw/brass-ish; good “big lead” source without Virus. |
| Korg **Minilogue XD** | Analog + custom OSC; multisample per preset/program. |
| Roland **SE-02** | Mono bass/lead; short note grid or single-root zones. |
| Korg **Volca FM2** | FM one-shots or small grids; sync/MIDI timing care. |
| Korg **Volca** (MS-20 filter / kick) | **One-shots** (kick, filtered noise); drum kit map, not keymap. |

---

## Samplers & groove (kit destination)

| Device | Role in pipeline |
|--------|------------------|
| **Akai MPC Live** | Primary **trap kit host**; import `instrument.map.json` / WAV folders / mpc-agent drafts. |
| **Akai MPC 500** | Legacy kit target; fewer zones, same map contract. |
| **Native Instruments Maschine+** | Sample library + performance; export WAVs → map. |
| Roland **SP-808** | Classic hip-hop sampling; one-shots/stems → MPC. |
| Roland **MV-1** | Looper/sampler; bounce stems or hits. |
| **Liven Lofi 12** | Lo-fi loops/one-shots; bounce to WAV for kits. |

---

## Drums & rhythm

| Device | Capture notes |
|--------|----------------|
| Arturia **DrumBrute** | Per-instrument **single WAV** or multi-velocity hits → drum map. |
| Roland **DR-660** | Drum machine one-shots. |
| Roland **DR-5** | Pattern/one-shot source; export hits not full ROM. |

---

## Not on this rack (Gearmulator optional)

No **Access Virus**, **Roland JP-8000**, **Ensoniq ASR**, or **Roland SU-700**.  
**Osirus / JE-8086 / 88emu** in Track E require **separate ROM + build**; use **Ultra Nova + MicroFreak + MPC Live** for day-to-day trap/multisample SKUs.

---

## Recommended capture flow (hardware)

```text
1. Choose preset on synth (or hit on DrumBrute/DR-660)
2. MIDI grid from DAW or MPC as master sequencer → record dry (or print HW FX)
3. Folder: note_XXX_vel_YYY.wav + manifest.tsv OR hand-edit instrument.map.json
4. Optional: apply_osirus_fx_to_session.py (software Virus bus)
5. mpc-agent map_to_mpc_program.py → MPC Live import
```

**Cloud VM:** DawDreamer captures **VST3 only** (e.g. WAVE-909). Hardware capture runs **locally**; commit **maps/plans/docs**, never WAVs.

---

## Priority SKUs (suggested first captures)

1. Ultra Nova — lead/brass preset, semitone grid.  
2. MicroFreak — bell/pluck preset.  
3. DrumBrute + DR-660 — trap drum one-shots into MPC Live.

---

## Related team memory

- [TEAM_AGENT_MEMORY.md](TEAM_AGENT_MEMORY.md)
- [.cursor/agent-memory/team-context.yaml](../.cursor/agent-memory/team-context.yaml)
