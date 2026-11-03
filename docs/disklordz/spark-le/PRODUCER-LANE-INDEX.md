# Producer → SparkLE lane index

Maps **trap/rap/R&B conversation** to **`presetId`** in `lane-registry.ts`. Archetypes only — not affiliated samples.

**Prompt inference:** If you leave preset on **MPC-ready 808** (or other neutral factory preset), the generator picks a lane from your brief via `coalesceSparkPreset()` ([lane-resolve.ts](../../../disklordz/website/src/lib/spark-le/lane-resolve.ts)).

## Tier A — trap hi-hat / top-loop architects

| Producer | Primary preset | Alt / alias |
|----------|----------------|-------------|
| Metro Boomin | `lane-metro-boomin` | — |
| Southside / 808 Mafia | `lane-808-mafia-southside` | `lane-southside` |
| Tay Keith | `lane-tay-keith` | — |
| Wheezy | `lane-wheezy` | — |
| Pi'erre Bourne | `lane-pierre-bourne` | — |
| ATL Jacob | `lane-atl-jacob` | — |
| Zaytoven | `lane-zaytoven` | — |
| TM88 | `lane-tm88` | — |
| Lex Luger | `lane-lex-luger` | — |
| DECAP (DTK aesthetic) | `lane-decap` | — |
| Pvlace | `lane-pvlace` | — |
| Gezin | `lane-gezin` | — |
| Outtatown | `lane-outtatown` | — |
| WondaGurl | `lane-wondagurl` | — |
| Lunch77 (clean reference) | `lane-splice-trap` | `lane-origin-sounds` |

## Tier B — mainstream / regional

| Producer | Preset |
|----------|--------|
| The Alchemist | `lane-alchemist` |
| Hit-Boy | `lane-hit-boy` |
| Mustard | `lane-mustard` |
| Cash Cobain | `lane-cash-cobain` |
| Craven & Daringer | `lane-griselda-craven` |
| Future | `lane-future` |
| London on da Track | `lane-london-on-da-track` |
| Purpz | `lane-purpz` |
| Johnny Juliano | `lane-johnny-juliano` |
| Cardo | `lane-cardo` |
| Cool & Dre | `lane-cool-dre` | `lane-cool-and-dre` |
| Timbaland | `lane-timbaland` |
| The Neptunes | `lane-neptunes` |

## Tier C — your core catalog (Memphis → phonk → trap)

See [LANES.md](LANES.md) — gangsta trap, Memphis, Atlanta, Cash Money, Drake-type, hardware TR-808/DR-660, Bay/Detroit, rage, phonk/wave, plugg, pack aesthetics.

## Tier D — Houston / screw / eras

| Vibe | Preset |
|------|--------|
| 90s Houston (general) | `lane-houston-90s` |
| 1990s Houston / SUC | `lane-houston-1990s` |
| 2000s Houston | `lane-houston-2000s` |
| Modern Houston trap | `lane-houston-modern` |
| Fat Pat type | `lane-fat-pat` | `lane-fat-phat` |
| DJ Screw | `lane-dj-screw` | factory `disklordz-screw` |
| Mike Dean / N.O. Joe | `lane-mike-dean-nojoe` |
| UGK / Pimp C pocket | `lane-ugk`, `lane-pimp-c` |
| Trae | `lane-trae` |

## Tier E — West / G-funk / strip

| Vibe | Preset |
|------|--------|
| Dogg Pound gangsta | `lane-dogg-pound` |
| 90s Snoop pocket | `lane-snoop-90s` |
| Dre Day / Chronic knock | `lane-dre-day-90s` |
| DJ Battlecat | `lane-dj-battlecat` |
| DJ Quik funk | `lane-dj-quik` |
| Strip / spend-that-shit anthem | `lane-strip-club-anthem` |

## Tier G — crunk / snap / boogie / regional punch

| Vibe | Preset |
|------|--------|
| Crunk | `lane-crunk` |
| Snap music | `lane-snap-music` |
| Boogie / 808 funk | `lane-boogie-808` |
| Slow Three 6 drag | `lane-three-six-slow-drag` |
| OG phonk | `lane-og-phonk` | `lane-of-phonk` |
| Slowed R&B / screw-soul | `lane-slowed-rnb` |
| Classic NOLA bounce | `lane-nola-bounce-classic` |
| Bay Area trap | `lane-bay-area-trap` | `lane-bay-trap` |
| Flint / Michigan | `lane-flint-michigan` |

## Tier F — trap soul / chill / alt R&B

| Vibe | Preset |
|------|--------|
| Trap soul (generic) | `lane-trap-soul` |
| Bryson Tiller type | `lane-bryson-tiller` |
| SZA type | `lane-sza` |
| Alina Baraz type | `lane-alina-baraz` |
| Chill trap | `lane-chill-trap` |
| Chillwave | `lane-chillwave` |
| Majestic Casual (not house 4×4) | `lane-majestic-casual` |
| Neo-soul live pocket | `lane-neo-soul-live` |
| Lo-fi jazz trap | `lane-lofi-jazz-trap` |
| Knxwledge type | `lane-knxwledge` |
| Jazz-funk fusion | `lane-jazz-funk-fusion` |
| Smooth jazz rap (generic) | `lane-smooth-jazz-rap` |
| Curren$y jazz rap | `lane-currensy-jazz-rap` |
| Pluggnb | `lane-pluggnb` |

## DNA + CI

| Resource | Path |
|----------|------|
| Pattern formulas | [TRAP-HAT-LOOP-DNA.md](TRAP-HAT-LOOP-DNA.md) |
| Half-time A/B workflow | [HALF-TIME-AB.md](HALF-TIME-AB.md) |
| Registry verify | `cd disklordz/website && npm run verify:spark-lanes` |
| Memory loop | [MEMORY-AND-CI.md](MEMORY-AND-CI.md) |
