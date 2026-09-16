# Disklordz Sector 01 — hardware capture list

From the brand chat. Check off as you record. Export **24-bit / 44.1 kHz WAV**, peaks ~−3 to −6 dB while tracking; master for release per [DISKLORDZ_RUN_SHEET.md](DISKLORDZ_RUN_SHEET.md).

**Gear referenced:** DR-660, DR-550, Drumbrute Impact, Liven Lofi-12, SP-808, Volca Kick, Volca FM2, MPC 500, MPC Live, Maschine+, MV-1, TR-808 VST.

---

## 1. Low-end (kicks & 808s)

- [ ] **Memphis Thump** — DR-660 Kick 1 & 2, internal level ~90% (clip the converter)
- [ ] **Analog Weight** — Volca Kick: short, long/sub, 3 drive stages
- [ ] **Disklordz 808** — TR-808 VST → SP-808 vinyl sim / distortion
- [ ] **FM Sub** — Volca FM2 deep metallic subs

## 2. Mid-range (snares & claps)

- [ ] **OG Snap** — DR-550 snare/clap layers
- [ ] **Industrial Snare** — Drumbrute Impact FM drum, Color on
- [ ] **Lo-Fi Crackle** — Liven Lofi-12 snares at lowest sample rate

## 3. Leads (cowbells & perc)

- [ ] **Holy Grail Cowbell** — DR-660 dry + wet through MV-1 reverb
- [ ] **Metallic Percs** — Volca FM2 (~10 hits)
- [ ] **MPC 500 Rim** — rimshot + woodblock

## 4. Tops & textures (hats & FX)

- [ ] **Brute Hats** — Drumbrute Impact closed/open (roller)
- [ ] **Tape Floor** — SP-808 ~30s vinyl sim hiss / crackle
- [ ] **12-bit Crash** — Liven Lofi-12 downsampled crash

---

## After capture

1. Trim, fades, rename: `DISKLORDZ_SECTOR01_<TYPE>_<NNN>.wav`  
2. Optional: 45 RPM resample trick ([brand doc](DISKLORDZ_SAMPLE_PACK_BRAND.md))  
3. Merge with factory renders: log params in `tools/data/disklordz.db` if you want one catalog  
4. Package via run sheet Square 3–4
