# SparkLE elite performance standard

**Version:** 1.0 · Companion to [SOP-012](../sops/SOP-012-spark-le-elite-system-charter.md) and phase SOPs 013–017.

## Purpose

Define **professional-grade** outcomes for hard **gangsta trap**, phonk, Memphis, southern, regional trap, and hardware lanes on **SparkLE** — loops and one-shots that hold up in real sessions, not just in a catalog.

## Performance tiers

| Tier | Scope | Elite bar |
|------|--------|-----------|
| **P0 — Session** | One SparkLE sit | 8 pads mapped; beat in ≤20 min; 30s export; review template filed |
| **P1 — Pack** | One zip / batch | Manifest + lineage; contracts green; ≥1 keeper or tagged learnings |
| **P2 — Lane** | One registry lane | 3+ keepers; stable params; blind A/B wins vs last default |
| **P3 — Family** | Memphis / ATL / South / Hardware / Regional / Rage | Monthly audit @ fixed BPM; keeper rate ≥35% |
| **P4 — System** | Full registry | Weekly cadence ([SOP-017](../sops/SOP-017-spark-le-phase-5-elite-leadership.md)); no red CI; RAG synced |

## Lane families (rotation)

Rotate **one family per week** in producer review so the catalog stays elite everywhere, not only Atlanta.

| Family | Lane IDs (manifest) | Loop priority |
|--------|---------------------|---------------|
| **Memphis** | DJ-PAUL, THREE-SIX-MAFIA, JUICY-J, SHAWTY-PIMP, EST-G, YOUNG-DOLPH | Crunch loops + cowbell tops |
| **Atlanta** | DJ-TOOMP, ZAYTOVEN, D-RICH, SHAWTY-REDD, GUCCI-MANE, MIKE-WILL, JEEZY | Trunk bounce + hat flutter loops |
| **South / Texas** | UGK, PIMP-C, DRUMMA-BOY, TI | Pocket loops, rim rides |
| **Chicago** | YOUNG-CHOP | Drill snap loops, kick-less tops |
| **Hardware** | DR-660, TR-808-CLONE | One-shot fidelity; align TR-808-CLONE with repo clone voice |
| **Regional trap** | BAY-AREA-TRAP, DETROIT-TRAP | Bright vs industrial loop stems |
| **Rage** | RAGE-TRAP | Short 808 + frantic hat loop stems |
| **Cash Money / NO** | MANNIE-FRESH, MASTER-P, BEATS-BY-THE-POUND | Bounce loops + clap stack; 92–108 BPM week |
| **Drake archetype** | DRAKE-TYPE | Polished kick-less tops + tight 808; hook-ready space |
| **Trap producer** | 808-MAFIA-SOUTHSIDE, PURPZ, JOHNNY-JULIANO, CARDO | Dark vs melodic vs cinematic A/B |
| **Jetlife / smooth** | JETLIFE, CURRENSY-JAZZ-RAP, WIZ, LARRY-JUNE, INVESTMENT-RAP | Low aggression KPI; rim/jazz contracts |
| **West / Bay** | DJ-QUIK, NIPSEY, BAY-HUSTLER, DOM-KENNEDY | Pocket + funk shuffle checks |
| **Strip anthem** | STRIP-CLUB-ANTHEM | clapWide + 2-bar loop mandatory |
| **909 hardware** | TR-909 | Pair with TR-808-CLONE A/B same session |
| **Houston** | HOUSTON-1990S, FAT-PAT, HOUSTON-2000S, HOUSTON-MODERN, DJ-SCREW | Era A/B: same loop at 72 vs 140 BPM double-time |
| **Detroit street** | HELLUVA, DOUGHBOYZ, DEJ-LOAF, TOP-SIDE, BIG-GERMZ | Snap vs smooth (Dej) A/B |
| **Cloud / lex / angry** | CLOUD-TRAP, LEX-LUGER, ANGRY-BEATS | Contrast cloud vs angry same BPM |
| **MS soul** | BIG-KRIT | Body snare + rim week |
| **Trap soul / chill** | TRAP-SOUL, NEO-SOUL-LIVE, LOFI-JAZZ-TRAP, KNXWLEDGE, JAZZ-FUNK-FUSION | Half-time A/B; rim-forward contracts |
| **Crunk / club** | CRUNK, SNAP-MUSIC, BOOGIE-808 | Wide clap + strip-loop DNA |
| **Memphis slow** | THREE-SIX-SLOW-DRAG, OG-PHONK | Decay + mud tag watch |
| **NOLA / Flint** | NOLA-BOUNCE-CLASSIC, FLINT-MICHIGAN | Bounce vs punch A/B @ 96 BPM |
| **G-funk west** | DOGG-POUND, SNOOP-90S, DRE-DAY-90S, DJ-BATTLECAT | Swing + rim week |
| **Core** | GANGSTA-TRAP, PHONK-TRAP (`midnight-circuit`) | Hard menace vs phonk dirt |

Full preset ids: [LANES.md](LANES.md) · code: `disklordz/website/src/lib/spark-le/lane-registry.ts`.

## Professional checklist (every pack)

- [ ] **Legal:** procedural/original only; keepers cleared
- [ ] **808:** sub mono-compatible; clone lane matches `lane-tr-808-clone` contract
- [ ] **Gangsta trap:** knock (pad 2) audibly separate from sub on phone speaker
- [ ] **Loops:** 2-bar minimum; kick-less stem included for Shawty Redd–style stacking
- [ ] **Manifest:** `lane`, `family`, `generator_version`, `lineage`
- [ ] **SparkLE:** overflow folder for 660/808 rim/cowbell alternates

## KPI targets (Phase 5 operating)

| KPI | Target |
|-----|--------|
| Median time-to-first-beat | ≤15 min |
| Keeper rate (kits) | ≥35% rising toward 50% on flagship lanes |
| Contract failures / merge | 0 on main |
| Review sessions / month | ≥4 |
| Lane family coverage / quarter | All families audited once |

## Continuous improvement (elite)

1. **Registry-first** — new lane = one row in `lane-registry.ts` + LANES.md family table + one RAG snippet.
2. **One fix per week** — highest tag count from reviews ([SOP-015](../sops/SOP-015-spark-le-phase-3-taste-keepers.md)).
3. **A/B before default swap** ([SOP-016](../sops/SOP-016-spark-le-phase-4-lineage.md)).
4. **Bump** `schema_version` in manifest when pad law or family taxonomy changes.

## Related

- [ARCHITECTURE.md](ARCHITECTURE.md) · [SOP-014](../sops/SOP-014-spark-le-phase-2-trust-contracts.md)
