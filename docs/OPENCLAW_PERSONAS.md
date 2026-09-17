# Band member persona engineering

Domain knowledge for **late-1970s / early-1980s vinyl-authentic** source material before lane transforms (phonk, screw, French touch, cyber funk).

## Instrument domain & recording chain

| Agent identifier | OpenClaw entry | Instrument domain | Emulated hardware & chain | Harmonic & acoustic specialty |
|------------------|----------------|-------------------|---------------------------|-------------------------------|
| **bandleader** | `disklordz_maestro` | Arrangement, direction, mix architecture | **Neve 8068** console, **Studer A800** 2-inch tape | Dynamic pacing, structural cohesion, analog coloration |
| **keys_arranger** | `vintage_keys` | Electric pianos, poly & lead synths | Rhodes, Wurlitzer, Minimoog, **DX7** | Minor 9th/11th voicings, modal shifts, portamento leads |
| **rhythm_section** | `rhythm_section` | Electric bass, acoustic drums, drum machines | **P-Bass**, Ludwig Vistalite, **Boss DR-660** | Muted flatwounds, syncopated swing, transient negative space |
| **vocal_topline** | `vocal_chops` | Lead vocal, BG harmonization, lyrics | **Shure SM7**, **Neumann U47**, **EMT 140** plate | Close-mic intimacy, three-part stacks, era lyrical tropes |

Machine-readable: [`recording_chains.yaml`](../disklordz-factory/openclaw/personas/recording_chains.yaml).

## Files

| Member | OpenClaw entry | SOUL (workspace) | YAML |
|--------|----------------|------------------|------|
| Isaac "The Maestro" Vance | `disklordz_maestro` | `workspaces/disklordz_maestro/SOUL.md` | [`isaac_vance.yaml`](../disklordz-factory/openclaw/personas/isaac_vance.yaml) |
| Marcus "Hertz" Dupree | `vintage_keys` | `workspaces/vintage_keys/SOUL.md` | [`marcus_dupree.yaml`](../disklordz-factory/openclaw/personas/marcus_dupree.yaml) |
| Bernard "Pocket" Washington | `rhythm_section` | `workspaces/rhythm_section/SOUL.md` | [`bernard_washington.yaml`](../disklordz-factory/openclaw/personas/bernard_washington.yaml) |
| Evelyn "Velvet" Hayes | `vocal_chops` | `workspaces/vocal_chops/SOUL.md` | [`evelyn_hayes.yaml`](../disklordz-factory/openclaw/personas/evelyn_hayes.yaml) |

Lane guardians (`artist_dl*`) add **YouTube/commerce BPM** rules on top — see [ARTIST_GREENLIGHTS.md](ARTIST_GREENLIGHTS.md).

## Two BPM layers

1. **Source session (Isaac):** **68–92 BPM** — optimize for analog groove and downstream chop/pitch/screw.
2. **Distribution lane:** DL001 118–124, DL002 140–160, DL006 60–95, DL004 112–128 — applied after source approval.

## Isaac — prompt compilation

Maestro merges fork summaries into **structured generative prompts** (console, mics, tape) for external audio engines. Store compiled prompts under `disklordz_maestro/briefs/` and Factory `DL-BRF-*` metadata.

## Stability before parallel spawn

[OPENCLAW_MIGRATION.md](OPENCLAW_MIGRATION.md) — migrate to 2.0 SQLite, `default: true` on Maestro, `openclaw doctor --session-sqlite inspect`.
