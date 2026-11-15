# MPC500 card export — 808 Lab kits onto the MPC500

**Version:** 1.0 · Rig: [MPC500-PC-ONLY.md](MPC500-PC-ONLY.md) · Procedure: [SOP-013](../sops/SOP-013-spark-le-phase-1-feed-spark.md) · Print: [MPC500-BENCH-CARD.md](MPC500-BENCH-CARD.md)

The MPC500 plays and sequences kits with no computer attached; the PC generates, verifies, and exports them. This doc is the last step: getting a kit folder onto the CF card in a form the MPC500 loads cleanly.

**Naming:** the operating model is the **MPC500 Lab hybrid**. Paths and IDs that say `spark-le` / `spark_le` / `verify:spark` are legacy names kept so links, manifests, and CI do not break.

---

## Pipeline

```text
lane → factory WAV (44.1 kHz, 16-bit mono) → verify:spark → manifest
  → export-mpc500-kit (short names + PADS.TXT) → CF card → MPC500 → review → keepers
```

```powershell
cd disklordz\website
npm run export:mpc500 -- --kit <kit folder> --out E:\DISKLORDZ\<CODE> [--ram-mb 16]
```

`<kit folder>` is any folder with `spark-le-manifest.json`, `OneShots/`, `Loops/` (e.g. [sprint-kits/session-3-smooth-jazz-rap](sprint-kits/session-3-smooth-jazz-rap/)).

Sprint kits 1–5 export as `TOD140`, `DS72`, `SJR86`, `FP84`, `UD144` — unique across kits, about 4.9 MB together, so all five fit in a stock 16 MB MPC500 at once. Without the prefix every kit has a `Tops_Kickless_2bar`, which collides (and exceeds 16 characters) as soon as two kits are in memory. `--out` defaults to `<kit>/MPC500/<CODE>`; point it at the CF card (card reader or MPC500 USB mode) to skip a copy.

Output:

| File | Purpose |
|------|---------|
| `SJR86_01SUB.WAV` … `SJR86_08FX.WAV` | Pad samples, names ≤ 16 chars, `A–Z 0–9 _` only |
| `SJR86_L1TOPS.WAV` | Loops (load to a free pad or bank B) |
| `PADS.TXT` | Pad sheet: which sample goes on A01–A12, BPM, swing note, RAM use |
| `mpc500-kit.json` | Machine-readable mapping (for a future `.PGM` writer) |

The exporter **fails** on anything the MPC500 cannot load cleanly: non-16-bit or non-44.1 kHz WAV, duplicate names, pads outside 1–12, or a kit larger than the RAM budget. It **warns** above 80% of the budget.

---

## MPC500 pad law (bank A, 12 pads)

Pads **A01–A08 are the charter 8-pad law** ([SOP-012](../sops/SOP-012-spark-le-elite-system-charter.md)) so every existing manifest stays valid. A09–A12 are optional extras.

| Pad | Role | Source in manifest |
|-----|------|--------------------|
| A01 | 808 sub | `spark_le.pads.1` |
| A02 | 808 knock | `spark_le.pads.2` |
| A03 | Snare body | `spark_le.pads.3` |
| A04 | Snap / clap | `spark_le.pads.4` |
| A05 | Closed hat | `spark_le.pads.5` |
| A06 | Open hat / air | `spark_le.pads.6` |
| A07 | Rim / perc | `spark_le.pads.7` |
| A08 | Cowbell / FX | `spark_le.pads.8` |
| A09 | Alt 808 (glide or retune) — role `808_alt` | `overflow[]` with `"mpc_pad": 9` |
| A10 | Perc 2 — role `perc_2` | `overflow[]` with `"mpc_pad": 10` |
| A11 | Vox / chop — role `vox_chop` | `overflow[]` with `"mpc_pad": 11` |
| A12 | Crash / riser / reverse — role `crash_riser` | `overflow[]` with `"mpc_pad": 12` |

Overflow entry shape: `{ "mpc_pad": 9, "role": "808_alt", "file": "OneShots/09_808_Alt.wav" }`.

---

## Verify on your unit (once)

These drive the exporter defaults; record the answers here.

- [ ] **RAM installed** (stock vs expansion) → pass `--ram-mb` if not 16. Long screw 808 tails and loops are what fill it.
- [ ] **Name length** shown on the sample/program screens (exporter assumes 16).
- [ ] **16-bit / 44.1 kHz WAV** loads; confirm a 24-bit file is refused (exporter blocks these anyway).
- [ ] **CF card** size and format that the MPC500 accepts.
- [ ] **OS version** (shown at boot).

---

## Not built yet

- **`.PGM` program file** — would assign all 12 pads in one load. Deliberately not written from memory: the binary layout must be confirmed against a program saved on *your* MPC500 first. `mpc500-kit.json` already holds the mapping the writer needs. Until then, assign pads by hand from `PADS.TXT` and **save the program on the MPC** — that saved `.PGM` is the reference file for the writer.
- **Auto copy to card** — `--out` to the card drive covers it for now.

---

## Related

[MPC500-PC-ONLY.md](MPC500-PC-ONLY.md) (MIDI out → `phonk_engine`) · [808-LAB-HYBRID.md](808-LAB-HYBRID.md) · [SPRINT-PROGRESS.md](SPRINT-PROGRESS.md)
