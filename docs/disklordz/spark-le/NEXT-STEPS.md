# 808 Lab — next steps (complete checklist)

**Updated:** 2026-11-03 · **Delivery:** on `main` (PR [#95](https://github.com/samuelfreemanjobs-hash/Instruments/pull/95) merged)

| Layer | Status |
|-------|--------|
| Repo / automation / MPC500 card exports | **Done** |
| Your MPC pass + `verify:producer-signoff` | **Remaining** |

---

## Automate (repo — no MPC)

From repo root:

```bash
./scripts/automate-808-lab.sh              # regenerate kits + MPC500 folders + verify + build
./scripts/automate-808-lab.sh --skip-regenerate   # when WAVs already committed
```

CI: workflow **808 Lab repo automation** (`workflow_dispatch` or PR on automation scripts).

**Human finish line:** `cd disklordz/website && npm run verify:producer-signoff` (scores, ≥3 keepers, Session 5 catalog note).

Pre-built MPC500 card folders: `sprint-kits/session-*/MPC500/<code>/` — copy to CF.

---

## A. MPC500 producer pass (your finish line)

**Print:** [MPC500-BENCH-CARD.md](MPC500-BENCH-CARD.md) · **CF export:** [MPC500-CARD-EXPORT.md](MPC500-CARD-EXPORT.md)

```powershell
cd disklordz\website
npm run export:mpc500 -- --kit ..\..\docs\disklordz\spark-le\sprint-kits\session-1-gangsta-trap --out E:\DISKLORDZ\TOD140
# repeat: DS72, SJR86, FP84, UD144
```

| # | Pack | BPM | Review file |
|---|------|-----|-------------|
| 1 | `session-1-gangsta-trap` | 140 | [TRAP-OR-DIE](reviews/2026-11-03_TRAP-OR-DIE_sprint1.md) |
| 2 | `session-2-dj-screw` | 72 | [DJ-SCREW](reviews/2026-11-03_DJ-SCREW_sprint2.md) |
| 3 | `session-3-smooth-jazz-rap` | 86 | [SMOOTH-JAZZ-RAP](reviews/2026-11-03_SMOOTH-JAZZ-RAP_sprint3.md) |
| 4 | `session-4-trunk-sermon` | 84 | [TRUNK-SERMON](reviews/2026-11-03_TRUNK-SERMON_sprint4.md) |
| 5 | `session-5-stretch-uk-drill` | 144 | [UK-DRILL](reviews/2026-11-03_UK-DRILL_sprint5.md) |

Per session:

- [ ] Pads **1–8** = manifest roles (same every kit)
- [ ] Sequence at manifest **BPM**
- [ ] **~30s** mix export from MPC
- [ ] **Vibe / Groove** scores (1–5)
- [ ] Tags: `kick-bleed` · `hat-mud` · `pocket-pull` · `perc-clutter`
- [ ] Session **5 only:** catalog **keep/drop** paragraph in review

**Keeper rule:** Vibe **and** Groove **≥ 4** → promote to [KEEPERS.md](KEEPERS.md) (target **≥ 3** total).

Optional reel (no PC): sequences A→B→C @ 140→72→86 — see bench card.

---

## B. After ears (one engineering cycle)

| Step | Action |
|------|--------|
| 1 | Count tags across 5 reviews → pick **one** dominant tag |
| 2 | One code/doc fix (factory, contract, or lane) addressing that tag |
| 3 | `cd disklordz/website && npm run verify:spark` |
| 4 | Re-export affected sprint kit(s) if audio changed |
| 5 | Log decision in [SYSTEM-PLAN.md](SYSTEM-PLAN.md) |

---

## C. Optional PC hybrid (same kits)

```powershell
cd disklordz\phonk-box\engine
python phonk_engine.py --manifest C:\path\to\session-1-gangsta-trap\spark-le-manifest.json --midi "Your MIDI In"
```

MPC **MIDI out** → PC **MIDI in**; audio from PC speakers separately.

---

## D. Later (pick when ready)

| Priority | Item | Doc |
|----------|------|-----|
| | Songwriting engine + MPC500 MIDI export | [songwriting-engine/README.md](../../../songwriting-engine/README.md) |
| | Memphis chop / screw PC app | [808-LAB-HYBRID.md](808-LAB-HYBRID.md) |
| | Keeper-driven contract thresholds | [SYSTEM-PLAN.md](SYSTEM-PLAN.md) Phase 2 |
| | 12-week rotation | [ELITE-PERFORMANCE.md](ELITE-PERFORMANCE.md) |
| | RAG from keeper notes | Phase 3 in SYSTEM-PLAN |

---

## Quick reference

| Need | Command / path |
|------|----------------|
| Regenerate all sprint WAVs | `npm run export:sprint-1-5` in `disklordz/website/` |
| CI gate | `npm run verify:spark` |
| Rig | [MPC500-PC-ONLY.md](MPC500-PC-ONLY.md) |
| Progress log | [SPRINT-PROGRESS.md](SPRINT-PROGRESS.md) |
| Index | [808-LAB.md](../808-LAB.md) |
