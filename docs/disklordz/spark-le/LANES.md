# SparkLE lane reference (style archetypes)

**Disclaimer:** Original procedural drum **archetypes** inspired by regional styles. **Not** affiliated with, endorsed by, or samples of any artist, producer, or brand.

**Source of truth (generation):** `disklordz/website/src/lib/spark-le/lane-registry.ts`  
**Elite standard:** [ELITE-PERFORMANCE.md](ELITE-PERFORMANCE.md)

All lanes use the **8-pad SparkLE map** in [SOP-012](../sops/SOP-012-spark-le-elite-system-charter.md).

## Core lanes (genre)

| Lane ID | Preset id | BPM hint | Sonic focus |
|---------|-----------|----------|-------------|
| **GANGSTA-TRAP** | `lane-mike-will`, `lane-jeezy`, etc. | 130–160 | Hard knock, clap stack, menace |
| **PHONK-TRAP** | `midnight-circuit` | 140–160 | Dirt, cowbell, Memphis snap |
| **VINTAGE-SOUTH** | `lane-ti`, `lane-drumma-boy`, `lane-ugk` | 72–104 | Rim, bounce, body snare |

## Memphis

| Lane ID | Preset id | BPM hint | Notes |
|---------|-----------|----------|--------|
| **DJ-PAUL** | `lane-dj-paul` | 65–90 / 130–180 | Horror-crunch, dark hats |
| **THREE-SIX-MAFIA** | `lane-three-six-mafia` | 65–95 | Hypnotic crush, perc on 8 |
| **JUICY-J** | `lane-juicy-j` | 130–160 | Dark bounce, triplet hat space |
| **SHAWTY-PIMP** | `lane-shawty-pimp` | 60–85 | Slow underground strip |
| **EST-G** | `lane-est-g` | 130–155 | Hard street trap |
| **YOUNG-DOLPH** | `lane-young-dolph` | 130–160 | Hard Memphis bounce |

## Atlanta

| Lane ID | Preset id | BPM hint | Notes |
|---------|-----------|----------|--------|
| **DJ-TOOMP** | `lane-dj-toomp` | 65–80 / 130–160 | Trunk ATL 808 *(alias: `lane-dj-doomp` → toomp)* |
| **ZAYTOVEN** | `lane-zaytoven` | 130–155 | Flutter hats, pocket |
| **D-RICH** | `lane-d-rich` | 130–155 | Trunk club weight (Zaytoven-adjacent) |
| **SHAWTY-REDD** | `lane-shawty-redd` | 70–88 / 140–176 | Dirty south **loops**, swing clap stack |
| **GUCCI-MANE** | `lane-gucci-mane` | 70–78 / 140–156 | Bounce, wide clap |
| **MIKE-WILL** | `lane-mike-will` | 130–160 | Minimal hats, hard snap |
| **JEEZY** | `lane-jeezy` | 68–76 / 136–152 | Anthem 808 weight |
| **DRUMMA-BOY** | `lane-drumma-boy` | 72–88 / 144–176 | Anthem crack *(also south)* |

## South / Texas

| Lane ID | Preset id | BPM hint | Notes |
|---------|-----------|----------|--------|
| **UGK** | `lane-ugk` | 70–90 / 140–180 | Houston pocket |
| **PIMP-C** | `lane-pimp-c` | 68–88 | Texas trunk, body snare |
| **TI** | `lane-ti` | 80–100 | Swagger, clean stack |

## Chicago

| Lane ID | Preset id | BPM hint | Notes |
|---------|-----------|----------|--------|
| **YOUNG-CHOP** | `lane-young-chop` | 130–150 | Drill-era pressure |

## Hardware

| Lane ID | Preset id | BPM hint | Notes |
|---------|-----------|----------|--------|
| **DR-660** | `lane-dr-660` | 90–120 | Machine punch, short decay |
| **TR-808-CLONE** | `lane-tr-808-clone` | any | Align with **repo 808 clone** voice (SP-1200 / factory 808 path) |

## Cash Money / New Orleans (old school)

| Lane ID | Preset id | BPM hint | Notes |
|---------|-----------|----------|--------|
| **MANNIE-FRESH** | `lane-mannie-fresh` | 92–110 / double | Cash Money bounce, Triggerman clap stack |
| **MASTER-P** | `lane-master-p` | 88–105 | No Limit era — simple hard 90s south |
| **BEATS-BY-THE-POUND** | `lane-beats-by-the-pound` | 90–108 | BBTP raw NO swing, rim + clap |

## Drake-type (charts archetype)

| Lane ID | Preset id | BPM hint | Notes |
|---------|-----------|----------|--------|
| **DRAKE-TYPE** | `lane-drake-type` | 70–95 / 140–150 | Polished 808, crisp hats, vocal space; prompt **hard drake** for trophy energy |

*Not affiliated with Drake, Cash Money, or listed artists — procedural style targets only.*

## Regional trap & rage

| Lane ID | Preset id | BPM hint | Notes |
|---------|-----------|----------|--------|
| **BAY-AREA-TRAP** | `lane-bay-area-trap` | 85–110 / 170+ | Hyphy-adjacent brightness |
| **DETROIT-TRAP** | `lane-detroit-trap` | 130–160 | Industrial street snap |
| **RAGE-TRAP** | `lane-rage-trap` | 140–160 | Short 808, frantic hats |

## artistLane codes

See `artistLane` on each preset in `lane-registry.ts` (e.g. `DL-TOOMP`, `DL-DJPAUL`, `DL-TR808`).

## Contract hints (Phase 2)

| Family | Emphasis |
|--------|----------|
| Memphis | SNARE-STACK dirt; dark HAT-CLARITY |
| Atlanta / gangsta | KNOCK vs SUB separation; clapWide on trunk lanes |
| Shawty Redd | LOOP-GRID swing; body snare |
| Hardware | Short KNOCK decay; DR-660 vs 808-CLONE peak bands |
| Rage | Minimal hatDecay; low 808 sustain |
| Bay vs Detroit | Bay brighter hats; Detroit harder snap |

Details: [SOP-014](../sops/SOP-014-spark-le-phase-2-trust-contracts.md) · [ELITE-PERFORMANCE.md](ELITE-PERFORMANCE.md)
