# NTS-1 mkII custom oscillators — Windows testing

On Windows you **flash and audition** units with **KORG KONTROL Editor** (or `loguecli`). Cross-compiling `.nts1mkiiunit` files needs **GNU Make + arm-none-eabi-gcc**, which this repo runs on **Linux / WSL2** or **GitHub Actions** — not natively in PowerShell.

## Fastest path (download CI build)

1. Open the latest successful **NTS-1 mkII build** workflow on GitHub (workflow `nts1-mkii-build.yml`).
2. Download the artifact **`nts1-mkii-units`** (zip).
3. Unzip to a folder you keep, e.g. `C:\dev\Instruments\release\nts1-custom-oscillators\mkII-nts1mkiiunit\`.
4. Continue with **Load on hardware** below.

## Build on your Windows PC (WSL2)

Use **WSL2 Ubuntu** in the same repo clone (e.g. `C:\dev\Instruments` → `/mnt/c/dev/Instruments`).

**One command** (from WSL, repo root):

```bash
cd /mnt/c/dev/Instruments
bash ./scripts/windows/build-mkii-wsl.sh
```

Equivalent manual steps:

```bash
cd /mnt/c/dev/Instruments
sudo apt-get update
sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi make python3 git
./tools/mkii-automate.sh bootstrap
export LOGUE_SDK="$(pwd)/.deps/logue-sdk"
./tools/build-all-bass-mkii.sh
./tools/package-nts1.sh
```

Packaged files: `release/nts1-custom-oscillators/mkII-nts1mkiiunit/*.nts1mkiiunit`

From **PowerShell**, you can open that folder after a WSL build:

```powershell
.\scripts\windows\prepare-mkii-test.ps1
```

## Catalog (genre bass / kick library)

| File | Style |
|------|--------|
| `tr808_kick_phonk.nts1mkiiunit` | Phonk 808 kick |
| `west_coast_moog_funk.nts1mkiiunit` | P-Funk / West Coast |
| `juno_106_rnb_bass.nts1mkiiunit` | Juno 80s R&B |
| `dilla_type_bass.nts1mkiiunit` | Dilla hip-hop bass |
| `moog_sub_phatty_bass.nts1mkiiunit` | Moog Sub Phatty |
| `prophet5_funk_rnb_bass.nts1mkiiunit` | Prophet-5 funk / R&B |
| `dx7_lately_bass.nts1mkiiunit` | DX7 Lately Bass |
| `sh101_babyface_bass.nts1mkiiunit` | SH-101 |
| `moog_voyager_se1_squelch.nts1mkiiunit` | Voyager / SE-1 squelch |
| `cardo_got_wings_gfunk.nts1mkiiunit` | G-funk sub |
| `larry_june_bay_cruise_bass.nts1mkiiunit` | Larry June cruise bass |
| `khalifa_haze_stoner_bass.nts1mkiiunit` | Stoner / haze sub |
| `juicy_j_three6_memphis_bass.nts1mkiiunit` | Three 6 / Memphis |
| `ensoniq_eps1_memphis_bass.nts1mkiiunit` | EPS-1 Memphis wavetable |

`template_osc` and `wavetable_osc` are SDK starters — use the rows above for musical tests.

Preset knob hints (0–100 on mkI manifests, scale to mkII 0–1023): `src/oscillators/<v1-folder>/manifest.json` → `_agent.presets`.

## Load on hardware (Windows)

1. **Firmware** NTS-1 mkII **≥ 1.0.0** (required for SDK 2.0 units).
2. USB-connect the NTS-1 mkII.
3. Install **KORG KONTROL Editor** from Korg’s site if needed.
4. In Kontrol Editor: select **NTS-1 mkII** → **Oscillator** user slot → **Load** → pick a `.nts1mkiiunit`.
5. **Write** / send to the device.
6. On the NTS-1: choose the **user oscillator** in the OSC menu; play keys and turn TYPE / A / B / C / D (mapped per unit `header.c` / manifest).

Only **one** custom oscillator is active at a time — keep the zip on disk and swap units between sessions.

## Optional: browser sim (DSP smoke test)

In **WSL**, after `bootstrap`, from one unit folder:

```bash
cd src/mkii/oscillators/tr808_kick_phonk
export LOGUE_SDK=/path/to/.deps/logue-sdk
make sim
```

Requires the SDK’s **emsdk** setup (see logue-sdk `platform/nts-1_mkii` docs). Use this for quick DSP checks; **hardware + Kontrol Editor** is the real acceptance test.

## Related docs

- [NTS1_INSTALL_TODAY.md](NTS1_INSTALL_TODAY.md) — mkI + mkII install overview  
- [nts1-mkii-porting.md](nts1-mkii-porting.md) — porting and `make` targets  
- [nts1-multi-bass-oscillators.md](nts1-multi-bass-oscillators.md) — full catalog table  