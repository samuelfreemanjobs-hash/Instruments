# Windows scripts (NTS-1 mkII)

Repo root on the operator machine is usually `C:\dev\Instruments`. Cross-compile runs in **WSL2**; flash and test in **PowerShell** + **KORG KONTROL Editor**.

## One command (recommended)

```powershell
cd C:\dev\Instruments
.\scripts\windows\build-and-prepare-mkii.ps1
```

WSL builds all `.nts1mkiiunit` files, then lists them and opens:

`release\nts1-custom-oscillators\mkII-nts1mkiiunit`

## Step by step

**After WSL has built** (or after unzipping a CI artifact into `release\`):

```powershell
cd C:\dev\Instruments
.\scripts\windows\prepare-mkii-test.ps1
```

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

GitHub Actions workflow `nts1-mkii-build.yml` uploads artifact **`nts1-mkii-units`**. Unzip into `release\nts1-custom-oscillators\`, then run `prepare-mkii-test.ps1`.