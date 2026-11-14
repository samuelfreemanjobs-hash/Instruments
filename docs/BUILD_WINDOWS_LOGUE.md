# Building logue SDK units on Windows (Sam)

This repo builds **NTS-1 / minilogue xd / Prologue** (SDK v1) and **NTS-1 mkII** (SDK v2) user oscillators from `src/`.

**Nothing in this document was hardware-tested on a Windows box in CI** — follow the steps below on your machine; artifacts from GitHub Actions are compile-only.

## What you need

| Item | Purpose |
|------|---------|
| [Git for Windows](https://git-scm.com/download/win) | Clone this repo and `logue-sdk` |
| **One** of the toolchain options below | ARM cross-compile |
| Optional: [Docker Desktop](https://www.docker.com/products/docker-desktop/) | Same toolchain as Korg’s Linux Docker image (recommended if MSYS2 is painful) |

### Toolchain option A — MSYS2 (native Windows, matches Korg scripts)

1. Install [MSYS2](https://www.msys2.org/).
2. Open **MSYS2 MINGW64** and run:
   ```bash
   pacman -S --needed git make patch unzip tar curl
   pacman -S --needed mingw-w64-x86_64-arm-none-eabi-gcc mingw-w64-x86_64-arm-none-eabi-binutils
   ```
3. Clone this repo and bootstrap the SDK once (from **Git Bash** or MSYS2, repo root):
   ```bash
   ./tools/mkii-automate.sh bootstrap
   ```
4. If Korg’s bundled gcc download fails, the build scripts fall back to MSYS2’s `arm-none-eabi-gcc` on `PATH` (same as Linux).

### Toolchain option B — Docker (logue-sdk container)

1. Install Docker Desktop.
2. Bootstrap SDK: `./tools/mkii-automate.sh bootstrap`
3. For **mkII** units, Korg’s image is the most reliable path when ARM’s gcc bundle will not download:
   ```bash
   ./tools/mkii-automate.sh docker tr808_kick_phonk
   cd .deps/logue-sdk/docker
   ./run_interactive.sh
   # inside container: build nts-1_mkii/inst-tr808_kick_phonk
   ```

### Toolchain option C — WSL2 Ubuntu

Same as Linux:

```bash
sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi make
./tools/mkii-automate.sh bootstrap
export LOGUE_SDK="$(pwd)/.deps/logue-sdk"
./tools/build-all-units.sh --all-platforms --mkii --package
```

## One-command build (Windows)

**Cloud compile + download (GitHub Actions):**

```powershell
.\scripts\antigravity-bridge\run-logue-units-ci.ps1 -Watch -Download
```

Or: `.\tools\windows\trigger-logue-units-ci.cmd -Watch -Download`

**Build on this PC:**

```powershell
.\tools\windows\build-all-logue.ps1
```

**Unattended:** set repo secret or local env `GH_WORKFLOW_TOKEN` (classic PAT with `workflow` scope), then run `scripts/trigger-logue-units-build.sh` from Task Scheduler. Weekly scheduled builds run via workflow `logue NTS-1 units` on `main` (Sundays 08:00 UTC).

Or double-click / `cmd`:

```bat
tools\windows\build-all-logue.cmd
```

Outputs:

- `release/nts1-custom-oscillators/` — install-ready folders by extension (`.ntkdigunit`, `.mnlgxdunit`, `.prlgunit`, `.nts1mkiiunit`)
- `release/logue-build-status.tsv` — per-unit build result

## Manual commands

```bash
export LOGUE_SDK="$(pwd)/.deps/logue-sdk"   # Git Bash / MSYS2 / WSL

./tools/build-all-units.sh --all-platforms   # all v1 osc + starter effects × 3 platforms
./tools/build-all-bass-mkii.sh               # NTS-1 mkII bass catalog
./tools/package-nts1.sh                      # copy into release/
```

Single unit:

```bash
./tools/build.sh oscillators/moog-voyager-se minilogue-xd
./tools/build-mkii.sh moog_voyager_se1_squelch
```

## CI artifacts (recommended for Sam)

No Windows compile required: GitHub Actions cross-compiles on Linux (same output as local WSL).

1. Open **Actions** → workflow **logue NTS-1 units** ([`.github/workflows/logue-nts1-units.yml`](../.github/workflows/logue-nts1-units.yml)).
2. Run **workflow_dispatch** on `main` (after PR merge) or use the artifact from the PR’s check run.
3. Download artifacts:
   - **`logue-custom-units`** — full tree (`mkI-ntkdigunit/`, `minilogue-xd-mnlgxdunit/`, `prologue-prlgunit/`, `mkII-nts1mkiiunit/`, `logue-build-status.tsv`)
   - **`logue-nts1-mkii-only`** — only `*.nts1mkiiunit` files

Artifacts are **not hardware-tested**.

## Catalog / DSP status

See [docs/LOGUE_UNIT_STATUS.md](LOGUE_UNIT_STATUS.md) for the per-unit finished / built matrix updated with each full build.
