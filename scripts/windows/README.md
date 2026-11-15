# Windows scripts (NTS-1 mkII)

Repo root on the operator machine is usually `C:\dev\Instruments`. Cross-compile runs in **WSL2**; flash and test in **PowerShell** + **KORG KONTROL Editor**.

## One command (recommended)

```powershell
cd C:\dev\Instruments
.\scripts\windows\build-and-prepare-mkii.ps1
```

WSL builds all `.nts1mkiiunit` files, then lists them and opens:

`release\nts1-custom-oscillators\mkII-nts1mkiiunit`

## Step by step (two lines after WSL build)

Operator copy: [docs/NTS1_MKII_WINDOWS_QUICKREF.md](../../docs/NTS1_MKII_WINDOWS_QUICKREF.md)

**After WSL has built** (or after unzipping a CI artifact into `release\`):

```powershell
cd C:\dev\Instruments
.\scripts\windows\prepare-mkii-test.ps1
```

Success → green list of ~16 units + Explorer at `release\nts1-custom-oscillators\mkII-nts1mkiiunit`. Yellow “No packaged mkII units” → run WSL `build-mkii-wsl.sh` first.

**WSL only:**

```bash
cd /mnt/c/dev/Instruments
bash ./scripts/windows/build-mkii-wsl.sh
```

## Execution policy

If scripts are blocked:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\windows\build-and-prepare-mkii.ps1
```

## Hardware

KORG KONTROL Editor → NTS-1 mkII → **Oscillator** → **Load** → pick a `.nts1mkiiunit` → **Write**.

Full guide: [docs/NTS1_MKII_WINDOWS_TEST.md](../../docs/NTS1_MKII_WINDOWS_TEST.md).

## Cloud / CI (no local compile)

Workflow **NTS-1 mkII oscillators** (`nts1-mkii-build.yml`) runs on **Ubuntu** and uploads **`nts1-mkii-units`** — the same `.nts1mkiiunit` files you load on Windows; no `windows-latest` runner needed.

```powershell
cd C:\dev\Instruments
gh auth login
.\scripts\windows\download-mkii-ci-artifact.ps1
.\scripts\windows\prepare-mkii-test.ps1
```

Or download the zip manually from Actions and unzip to `release\nts1-custom-oscillators\`.