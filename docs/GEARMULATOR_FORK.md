# Gearmulator fork (your copy)

Use a **fork** when you want your own remote for multisampling experiments, custom scripts, or long-lived pins—without pushing to [dsp56300/gearmulator](https://github.com/dsp56300/gearmulator).

The Instruments monorepo keeps Gearmulator as a **git submodule** at `gearmulator-lane/gearmulator/`.

**Canonical remote (this repo):** https://github.com/samuelfreemanjobs-hash/gearmulator.git  

Sync fixes from upstream when needed (section 4). To repoint after a fork rename:

```bash
./scripts/gearmulator/repoint-submodule-to-fork.sh https://github.com/samuelfreemanjobs-hash/gearmulator.git
```

## 1. Clone with submodule

```bash
git submodule update --init --recursive gearmulator-lane/gearmulator
```

That updates `.gitmodules` and the submodule’s `origin` remote. Nested upstream submodules (dsp56300, JUCE, etc.) still init with:

```bash
git submodule update --init --recursive gearmulator-lane/gearmulator
```

## 3. Work on the fork

```bash
cd gearmulator-lane/gearmulator
git checkout -b feature/multisample-tools   # example
# edit, commit, push to your fork
git push -u origin feature/multisample-tools
```

Open PRs on **your fork** (or upstream if you contribute back). Keep **GPLv3** compliance if you distribute binaries or combined works.

## 4. Stay synced with upstream

Inside the submodule:

```bash
cd gearmulator-lane/gearmulator
git remote add upstream https://github.com/dsp56300/gearmulator.git 2>/dev/null || true
git fetch upstream
git merge upstream/main   # or rebase, per your preference
git push origin main
```

Then bump the submodule SHA in Instruments when you want the monorepo to pin that version:

```bash
cd gearmulator-lane/gearmulator && git checkout main && cd ../..
git add gearmulator-lane/gearmulator
git commit -m "chore: bump gearmulator submodule"
```

## Policy (Instruments repo)

| Item | Rule |
|------|------|
| ROM / firmware dumps | **Never** commit to Instruments or your fork’s public branches |
| Multisamples from LLE plugins | Your responsibility for rights/redistribution; see [GEARMULATOR_LANE.md](GEARMULATOR_LANE.md) |
| JD Upgraded `Source/` | Do not merge GPLv3 emulation into proprietary plugin code without a license WO |

## Related

- [gearmulator-lane/README.md](../gearmulator-lane/README.md) — clone & build  
- [GEARMULATOR_LANE.md](GEARMULATOR_LANE.md) — Track E policy  
