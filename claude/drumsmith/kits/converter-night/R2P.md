# R2P: Converter Night (CNV)

**Date:** 2026-11-17 · **Status:** draft · **Skills:** research-to-product → product-design

## Source briefs

- User machine research (DR-660 rate-drop, vintage palette) — chat 2026-11-17 `[inspected]`
- Storefront tree + ReadMe draft (sanitized on execute) `[inspected]`
- Deep-research upgrades (wedge, memphis-tape profile, RELEASE_GATE) `[inspected]`
- 26 unique hardware WAV archives under Cursor uploads `[executed` inventory]

## Hero wedge (store copy)

**90s Memphis rap / phonk drums**: (1) rate-dropped subs, (2) 12-bit / rompler snap, (3) cutting EDGE hats. Internal banks stay off the product title.

Producer-archetype ear lanes (no artist names on SKUs): Converter Crawl · Bitcrush Bed · Horror Snap · Club Bounce — `recipes/MEMPHIS_ARCHETYPES.md`.

## Decisions

| Finding (tag) | Action | Product field | Notes |
|---------------|--------|---------------|-------|
| DR-660 rate-drop −12…−24, choke, pitched 808/cowbell language `[inspected]` | use | Bank RATE + `loops/cnv-dr660-*.json` | **No DR-660 WAV in uploads** — TR-808 Soft/Dist BD + 808cowbl as proxies `[executed]`; see `references/dr660/STAGED.md` |
| SP-1200 / Drumulator 12-bit `[inspected]` | use | Bank BIT12 | Text-only until WAV; honest label |
| HR-16 A/B uploads `[executed]` | use | Bank ROMP | Primary RE |
| TR-808 lofi set `[executed]` | use | RATE + dirt cowbell/hats | Not an “808 pack” |
| TR-909 lofi set `[executed]` | use | Bank EDGE | Layer hats over subs |
| TR-606 + KPR-77 `[executed]` | use | Bank ANLG | |
| DMX `[executed]` | use | Bank CAVE | Room clap/snare |
| TR-707 / TR-505 / DR-5 `[executed]` | use | Punch PCM | |
| RX-5/11/15 `[executed]` | use | Papery snare / woody tom | |
| Kawai R-50/R-100 `[executed]` | use | Metallic ring clap/perc | |
| TR-727 + RT-123 + DDD5 Latin `[executed]` | use | Perc ≤10 | Cap |
| Simmons SDS1000/SDS1 `[executed]` | use | Bank SIMM | Subsample |
| XE8, DDD1, RZ-1, Drumtraks, DR-55 `[executed]` | use | Fill | |
| Roland R-8 ~243 WAV `[executed]` | use | Subsample ≤12 exemplars | Do not inflate |
| Boss DR-5 duplicate zip `[executed]` | reject | — | Same as `Boss_DR-5__2__0615` |
| Trademarked machine product titles `[inspected]` | reject | Marketing | **Night Crawl** store title only |
| 24-bit claim if factory is PCM16 `[inspected]` | reject | Delivery | Spec **44.1 kHz / PCM16 / mono** to match verify_kit |
| Drift-phonk-only factory `[inspected]` | use | Genre `memphis-tape` + CNV recipes | Deep-research #2 |
| Illugen SaaS bridge `[inspected]` | park | Post v1 | |

## Rejected / parked

- Shipping uncleared reference WAVs in `01_RAW…`
- “26 machines” store positioning
- Planet Phatt melodic multi; SR-16 until WAV
- JUCE VST / waveform GUI (out of SKU scope)

## INTAKE draft

See `intake-answers.md` (commercial intent; devices UNVERIFIED; price HITL).

## Spec status

`draft` — kit maxed to **512** (drums @ 3× guide + full useful FX set); **not** ship-candidate until RELEASE_GATE HITL.

## Next skills

- [x] product-design (kit-spec)
- [ ] audio-dsp-engineering / RE meters on staged refs
- [x] native_factory / memphis-tape render (W2 — see `W2_RENDER.md`)
- [x] audio-analysis + CALIPER draft (`CALIPER_NOTES.md`) — HITL still open
- [x] true 2-bar loops (`W2B_TRUE_LOOPS.md` + listen preview zip)
- [ ] customer-and-competition-intel (price)
- [ ] Sam: ear · legal · price · cover

## Reference inventory (26 unique)

| Ref id | Archive | Bank focus |
|--------|---------|------------|
| xe8 | Akai_XE8_4732.zip | Fill / electro toms |
| dr5 | Boss_DR-5__2__0615.zip | Punch PCM |
| dr55 | Boss_DR-55_5dce.zip | Sparse ANLG-era |
| ddd1 | Korg_DDD1_2d5b.zip | Fill |
| ddd5 | Korg_DDD5_358a.zip | Latin (cap); E.Bass texture only |
| rz1 | Casio_RZ-1_0ceb.zip | Fill |
| sds1000 | Simmons_SDS1000_ad30.zip | SIMM |
| sds1 | Simmons_SDS1_8cba.zip | SIMM |
| drumtraks | Sequential_Circuits_Drumtraks_064d.zip | Fill |
| rt123 | Zoom_RT-123_c435.zip | Latin/perc |
| r100 | Kawai_R-100_2778.zip | Metallic |
| r50 | Kawai_R-50_f265.zip | Metallic |
| rx15 | Yamaha_RX-15_c6fe.zip | RX |
| rx11 | Yamaha_RX-11_ffdd.zip | RX |
| rx5 | Yamaha_RX-5_57eb.zip | RX |
| tr727 | Roland_TR727_9adf.zip | Latin |
| dmx | Oberheim_DMX_4c9f.zip | CAVE |
| hr16a | Alesis_HR16A_5632.zip | ROMP |
| hr16b | Alesis_HR16B_d6a0.zip | ROMP |
| tr808 | Roland_TR808_lofi_set_84b4.zip | RATE |
| tr909 | Roland_TR909_lofi_set_8d01.zip | EDGE |
| tr505 | Roland_TR505_02e2.zip | Punch PCM |
| tr707 | Roland_TR707_7d95.zip | Punch PCM |
| tr606 | Roland_TR606_8f72.zip | ANLG |
| kpr77 | Korg_KPR77_d3bd.zip | ANLG |
| r8 | Roland_R8_2d4d.zip | Subsample ≤12 |

Duplicate ignored: `Boss_DR-5_9f45.zip`.
