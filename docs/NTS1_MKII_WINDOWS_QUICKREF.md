# NTS-1 mkII — Windows quick reference

Those two lines are correct **after WSL has built and packaged** the units (or after you unzip the CI artifact into `release\`).

## PowerShell (repo root)

```powershell
cd C:\dev\Instruments
.\scripts\windows\prepare-mkii-test.ps1
```

**If it works:** green list of ~16 `.nts1mkiiunit` files and Explorer opens:

`C:\dev\Instruments\release\nts1-custom-oscillators\mkII-nts1mkiiunit`

**If it exits with yellow text “No packaged mkII units”:** run WSL first:

```bash
cd /mnt/c/dev/Instruments
bash ./scripts/windows/build-mkii-wsl.sh
```

Then run the PowerShell script again.

**If execution policy blocks the script:**

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\windows\prepare-mkii-test.ps1
```

**All-in-one** (WSL build + prepare in one PowerShell session):

```powershell
cd C:\dev\Instruments
.\scripts\windows\build-and-prepare-mkii.ps1
```

## Load on hardware

**KORG KONTROL Editor** → NTS-1 mkII → **Oscillator** → **Load** → pick a file from that folder → **Write**.

Firmware **≥ 1.0.0** required.

## No WSL — build on GitHub Actions

1. GitHub → **Actions** → **NTS-1 mkII oscillators** → **Run workflow** (or wait for green run on `main`).
2. Download artifact **`nts1-mkii-units`**, unzip into `release\nts1-custom-oscillators\`.

**PowerShell (GitHub CLI):**

```powershell
cd C:\dev\Instruments
git pull origin main
gh auth login
.\scripts\windows\diagnose-mkii-download.ps1   # optional debug
.\scripts\windows\download-mkii-ci-artifact.ps1
.\scripts\windows\prepare-mkii-test.ps1
```

**“No successful run”:** run `git pull`, `gh auth login`, then `gh workflow run nts1-mkii-build.yml -R samuelfreemanjobs-hash/Instruments --ref main` and retry after green.

**Download EOF / fails:** download the zip in the browser from that Actions run, or WSL: `bash ./scripts/windows/download-mkii-ci-artifact.sh` or `bash ./scripts/windows/build-mkii-wsl.sh`.

More detail: [NTS1_MKII_WINDOWS_TEST.md](NTS1_MKII_WINDOWS_TEST.md)