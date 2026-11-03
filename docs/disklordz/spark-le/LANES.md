# SparkLE lane reference (style archetypes)

**Disclaimer:** These are **original procedural drum archetypes** inspired by well-known southern/trap production *styles*. They are **not** affiliated with, endorsed by, or samples of any artist or producer. Use for internal lane IDs, manifests, and generation params only.

All lanes use the **8-pad SparkLE map** in [SOP-012](../sops/SOP-012-spark-le-elite-system-charter.md).

## Core lanes (genre)

| Lane ID | Preset id (SaaS) | BPM hint | Sonic focus |
|---------|------------------|----------|-------------|
| **GANGSTA-TRAP** | *(use producer lanes below or `mpc-ready-808`)* | 130–160 | Hard knock, clap stack, menace |
| **PHONK-TRAP** | `midnight-circuit` | 140–160 | Dirt, cowbell, memphis snap |
| **VINTAGE-SOUTH** | `lane-ti`, `lane-drumma-boy` | 72–104 | Rim, bounce, body snare |

## Producer / artist archetype lanes

| Lane ID | Preset id | BPM hint | Pad emphasis | Loop / groove notes |
|---------|-----------|----------|--------------|---------------------|
| **JEEZY** | `lane-jeezy` | 68–76 / 136–152 | Long 808 (1), dark knock (2), anthem snare (3–4) | Slow march or double-time; space for ad-libs |
| **GUCCI-MANE** | `lane-gucci-mane` | 70–78 / 140–156 | Bouncy sub, quirky perc (7–8), wide clap (4) | Swung bounce; odd percussion on 8 |
| **MIKE-WILL** | `lane-mike-will` | 130–160 | Tight tick (2), hard snap (4), FX/tag slot (8) | Minimal hat grid, heavy downbeats |
| **DRUMMA-BOY** | `lane-drumma-boy` | 72–88 / 144–176 | Crack snare (4), rim (7), orchestral hit / FX (8) | Anthem builds; snare cracks |
| **ZAYTOVEN** | `lane-zaytoven` | 130–155 | Lighter knock (2), flutter hats (5–6), shaker-like (7) | Fast hat patterns; ATL pocket |
| **TI** | `lane-ti` | 80–100 / 160–200 | Clean stack (3–4), confident clap width, crisp hats | King-of-the-South swagger; less mud |
| **DJ-DOOMP** | `lane-dj-doomp` | 65–90 / 130–180 | Low muddy sub (1), crushed snare (3–4), dark hats | Slow Memphis crunch / strip cadence *(label as you use it; adjust brief if this is a different reference)* |

## Mapping to Disklordz `artistLane` codes

| artistLane | Lanes |
|------------|--------|
| `DL-JEEZY` | JEEZY |
| `DL-GUCCI` | GUCCI-MANE |
| `DL-MIKEWILL` | MIKE-WILL |
| `DL-DRUMMA` | DRUMMA-BOY |
| `DL-ZAYTOVEN` | ZAYTOVEN |
| `DL-TI` | TI |
| `DL-DOOMP` | DJ-DOOMP |

## Contract hints (Phase 2)

See [SOP-014](../sops/SOP-014-spark-le-phase-2-trust-contracts.md). Producer lanes inherit core contracts with offsets:

- **JEEZY / DRUMMA-BOY:** longer 808 decay band; snare crack peak on pad 4  
- **ZAYTOVEN:** HAT-CLARITY on 5–6 (flutter) stricter grid  
- **MIKE-WILL:** KNOCK transient shorter; less sub on pad 2  
- **GUCCI-MANE:** clapWide manifest target higher  
- **DJ-DOOMP:** SNARE-STACK dirt allowed; HAT-CLARITY relaxed vs TI  

## Related

- [SOP-012](../sops/SOP-012-spark-le-elite-system-charter.md) · [prompt-params.ts](../../../disklordz/website/src/lib/generation/prompt-params.ts) · [presets.ts](../../../disklordz/website/src/lib/presets.ts)
