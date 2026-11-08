# Antigravity: trigger **logue NTS-1 units** on GitHub Actions

Sam asked for a **Run workflow** on the **logue NTS-1 units** workflow. Cursor Cloud cannot click the GitHub UI; **you run this on the Antigravity Windows workspace** (GitHub CLI or browser).

## Prerequisite

- `gh auth login` (repo scope) on this PC
- Repo root: e.g. `C:\dev\Instruments`

## Option A — GitHub CLI (preferred)

Until PR **#113** merges, build from the feature branch:

```powershell
cd C:\dev\Instruments
git fetch origin
gh workflow run "logue NTS-1 units" --repo samuelfreemanjobs-hash/Instruments --ref cursor/logue-osc-build-windows-13db
```

After merge, use `main`:

```powershell
gh workflow run "logue NTS-1 units" --repo samuelfreemanjobs-hash/Instruments --ref main
```

Or run the helper:

```powershell
.\scripts\antigravity-bridge\run-logue-units-ci.ps1
```

Watch the run:

```powershell
gh run list --workflow="logue NTS-1 units" --limit 3
gh run watch
```

Download artifacts (after success):

```powershell
$runId = (gh run list --workflow="logue NTS-1 units" --limit 1 --json databaseId -q '.[0].databaseId')
gh run download $runId -D "$env:USERPROFILE\Downloads\logue-custom-units"
```

Artifacts: **`logue-custom-units`** (all platforms + mkII) and **`logue-nts1-mkii-only`**.

## Option B — Click **Run workflow** in the browser

1. Open: https://github.com/samuelfreemanjobs-hash/Instruments/actions/workflows/logue-nts1-units.yml  
2. **Run workflow** → Branch: `cursor/logue-osc-build-windows-13db` (or `main` after merge) → **Run workflow**  
3. Open the new run → **Artifacts** → download zips.

## Option C — `repository_dispatch` (automation)

If the workflow on the chosen ref includes `repository_dispatch` type `build-logue-units`:

```powershell
gh api repos/samuelfreemanjobs-hash/Instruments/dispatches `
  -f event_type=build-logue-units `
  -f client_payload[ref]=cursor/logue-osc-build-windows-13db
```

## Done handoff

When the run is **green**, publish back to Cursor:

```powershell
.\scripts\antigravity-bridge\publish-handoff.ps1 `
  -Direction antigravity_to_cursor `
  -WorkOrderId WO-2026-LOGUE-GHA-001 `
  -Status done `
  -Notes "logue NTS-1 units GHA run <URL>; artifacts in Downloads\logue-custom-units"
```

Replace `<URL>` with the Actions run link.

**Not hardware-tested** — artifacts are compile-only.
