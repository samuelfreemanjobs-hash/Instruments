# Trap hi-hat, perc & top-loop DNA

**Version:** 1.0 · Pair with [lane-registry.ts](../../../disklordz/website/src/lib/spark-le/lane-registry.ts) and [PRODUCER-LANE-INDEX.md](PRODUCER-LANE-INDEX.md).

## Grid notation

| Symbol | Meaning |
|--------|---------|
| `x` | Closed hat hit |
| `o` | Open hat / ride |
| `.` | Rest |
| `(roll)` | 32nd or 64th burst |
| `T` | Triplet subdivision in bar |
| `V↑` / `V↓` | Velocity accent / ghost |

Default **2-bar** loop for SparkLE export. BPM = manifest BPM.

## Layer stack (top loop stem)

1. **CH** — closed hat (pad 5)  
2. **OH** — open / air (pad 6)  
3. **PERC** — rim, stick, shaker (pad 7)  
4. **FX** — cowbell, click, foley (pad 8)  

Kick/snare on separate stems for layering (Shawty Redd / strip anthem rule).

---

## Pattern formulas by architect

### Metro Boomin (`lane-metro-boomin`)

| Element | Formula |
|---------|---------|
| Hats | Bar1: `x.x.x.x.x.x.x.x` (16ths); Bar2: `(roll)` on beat 4 into downbeat |
| Pocket | Slight late swing on off-16ths (58–62% swing) |
| Perc | Sparse rim on beat 3 only |
| 808 | Long tail; hats **duck** under sub |

### Southside / 808 Mafia (`lane-808-mafia-southside`, alias `lane-southside`)

| Element | Formula |
|---------|---------|
| Hats | **32nd rolls** ending beats 2 and 4: `xxxxxxx` + stop |
| Alt | Double-time 16ths full bar for aggression |
| Perc | Low rim `x.....x.....` triplet feel |
| Energy | Unquantized ±8 ms on roll last 3 hits |

### Tay Keith (`lane-tay-keith`)

| Element | Formula |
|---------|---------|
| Hats | **Staccato**: `x.x.x.x` (8th-note accents) + silence gaps |
| Kick sync | Hat only on kick offsets, not every 16th |
| Perc | Minimal; clap stack separate stem |
| BPM | 130–150; space for vocal |

### Wheezy (`lane-wheezy`)

| Element | Formula |
|---------|---------|
| Hats | Syncopated **dropouts**: `x.x.x...x.x.x...` (missing & of 2) |
| Pocket | Push beat 2 late, pull beat 4 early |
| Perc | Light shaker 16ths under CH |
| Feel | “Pull” before downbeat of bar 2 |

### Pi'erre Bourne (`lane-pierre-bourne`)

| Element | Formula |
|---------|---------|
| Hats | Bouncy 16ths with **velocity ramp** into snare |
| FX | Short riser on pad 8 before bar line |
| Space | Leave beat 1 of bar 2 half-empty |

### ATL Jacob (`lane-atl-jacob`)

| Element | Formula |
|---------|---------|
| Hats | Dark 16ths + **1 open hat** sustained beat 4 |
| Perc | Eerie tick on `e` of 3 |
| Mix | High-pass hats above 6 kHz for air |

### Zaytoven (`lane-zaytoven`)

| Element | Formula |
|---------|---------|
| Hats | **Flutter** `(roll)` 32nds beats 1–2, straight 16ths beats 3–4 |
| Perc | Shaker syncopated T T T |

### Mustard (`lane-mustard`)

| Element | Formula |
|---------|---------|
| Hats | **Ratchet** off-beat: `.x.x.x.x.x.x.x` |
| Clap | Layer wide on 2 and 4 (pad 4 stem) |
| BPM | Often 95–105 feel or double |

### Hit-Boy (`lane-hit-boy`)

| Element | Formula |
|---------|---------|
| Hats | Boom-bap **8ths** with swing 66% |
| Perc | Live rim shuffle `x..x..x..x..` |
| Knock | Short 808 (pad 2) not long sub |

### The Alchemist (`lane-alchemist`)

| Element | Formula |
|---------|---------|
| Hats | **Drum-light** — rim + kick; hats optional ghosts |
| Loop type | Top = perc + vinyl noise, not busy hats |
| Griselda adj | See `lane-griselda-craven` |

### Craven & Daringer / Griselda (`lane-griselda-craven`)

| Element | Formula |
|---------|---------|
| Hats | Sparse or **drum-less** top (rim + FX only) |
| Formula | Bar: `....x.......x...` rim; no 16th grid |

### Cash Cobain (`lane-cash-cobain`)

| Element | Formula |
|---------|---------|
| Hats | Drill-adjacent **straight 16ths** + sample chop feel |
| Bounce | Syncopated kick pattern implied in hat gaps |

### WondaGurl (`lane-wondagurl`)

| Element | Formula |
|---------|---------|
| Perc | Distorted **foley** on pad 7–8 |
| Hats | Irregular: `x.xx.x.x.xx.x` |
| Texture | Bitcrush 12-bit on hat stem |

### DECAP (`lane-decap`)

| Element | Formula |
|---------|---------|
| Top | **Saturated** acoustic hat + Foley layer |
| Formula | 16ths V↑ on downbeats; parallel comp on stem |

### Pvlace (`lane-pvlace`)

| Element | Formula |
|---------|---------|
| Hats | **Pan motion** L/R every 2 hits |
| Perc | Rim pitch-bend down on beat 4 |

### Outtatown (`lane-outtatown`)

| Element | Formula |
|---------|---------|
| Hats | **Rage** straight 16ths @ 145–160 |
| FX | Bleeps on pad 8 every half bar |

### Gezin (`lane-gezin`)

| Element | Formula |
|---------|---------|
| Hats | Dark **mid-hats** 16ths low velocity |
| Perc | Metallic tick + reverse swell |

### Three 6 / Project Pat (`lane-three-six-mafia`, `lane-project-pat`)

| Element | Formula |
|---------|---------|
| Hats | Dark 16ths with **stop** before snare |
| Perc | Cowbell / FX on pad 8 downbeat bar 2 |
| Snap | Heavy unquantized backbeat |

### DJ Squeaky (`lane-dj-squeaky`)

| Element | Formula |
|---------|---------|
| Hats | Bright **16ths** + open on 4 |
| Perc | DJ-style party hits on & of 2 |

### Lunch77 / pack clean (`lane-splice-trap`, `lane-origin-sounds`)

| Element | Formula |
|---------|---------|
| Hats | **Industry 16th** grid, even velocity |
| Use | Reference layer under custom lane |

---

## Layering recipe (any lane)

1. Generate **kick-less top** 2 bars @ manifest BPM.  
2. Add **CH** loop → **OH** on offbeats → **PERC** fill bar 2.  
3. A/B against **one-shot** pads 5–8 in SparkLE.  
4. Score **Groove** + tag `hat-roll` | `pocket` | `mud` in [review template](../templates/TEMPLATE-spark-le-session-review.md).

---

## MIDI DNA ids (for future `loop-render`)

| `dna_template_id` | Lane |
|-------------------|------|
| `metro-roll-bar4` | METRO-BOOMIN |
| `southside-32-roll` | 808-MAFIA-SOUTHSIDE |
| `tay-staccato-8` | TAY-KEITH |
| `wheezy-dropout` | WHEEZY |
| `mustard-ratchet` | MUSTARD |
| `outtatown-rage-16` | OUTTATOWN |

---

## Related

- [spark-trap-loop-architect skill](../../../.claude/skills/spark-trap-loop-architect/SKILL.md)  
- [SOP-014 Phase 2 contracts](../sops/SOP-014-spark-le-phase-2-trust-contracts.md) (`HAT-CLARITY`, `LOOP-GRID`)
