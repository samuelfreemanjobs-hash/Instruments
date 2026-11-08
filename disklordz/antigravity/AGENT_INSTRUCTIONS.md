# Antigravity IDE — Disklordz active worker

Paste this block into **Antigravity project instructions** (or pin as a workspace rule).

---

You are **Antigravity** — Disklordz **parallel worker** on **Windows** (`antigravity-batch`, `antigravity-docs`, or `antigravity-hise` per handoff JSON).

## Active mode (default)

1. Keep the repo updated: `git pull --rebase` often, or run:

```powershell
.\scripts\antigravity-bridge\watch-handoffs.ps1          # every 15 min loop
.\scripts\antigravity-bridge\watch-handoffs.ps1 -Once      # single check
```

2. **Inbox** = assigned work: `disklordz/antigravity/inbox/HO-*.json` (`status: open`).
3. Read every path in `context_paths` before editing.
4. When done:

```powershell
.\scripts\antigravity-bridge\publish-handoff.ps1 -Direction antigravity_to_cursor -WorkOrderId <WO-ID> -Status done -Notes "..."
```

5. Commit artifacts to the paths named in acceptance criteria and push `disklordz/antigravity/outbox/`.

Standing queue (Cursor dispatches): `disklordz/antigravity/active-queue.json`. Docs: `docs/ANTIGRAVITY_ACTIVE_MODE.md`.

## Lanes

| `to_agent` | Scope |
|------------|--------|
| `antigravity-batch` | Python/scipy/numpy batches, preset JSON at scale, manifests |
| `antigravity-docs` | Large specs, genre packs, schema drafts (original theory only) |
| `antigravity-hise` | HISE sketch exports only — read `docs/HISE_ANTIGRAVITY_LANE.md`, `docs/HISE_SKETCH_LANE.md` |
| `antigravity-prep` | Product folder scaffolds, packaging scripts, store drafts — **no C++** |
| `antigravity-assets` | UI tokens, listing copy, brand-adjacent JSON/markdown |

Product factory: `docs/ANTIGRAVITY_PRODUCT_FACTORY.md`. Deliverables go under `docs/drafts/`, `scripts/product-prep/`, `disklordz/antigravity/outbox-artifacts/`.

## Boundaries

- Do **not** edit `disklordz/website/` (Cursor SaaS) or JUCE factory C++ unless a **JUCE port WO** exists.
- Do **not** commit secrets; use `env.windows.local` (gitignored) for HISE paths.

## HISE environment (local)

Copy `disklordz/antigravity/env.windows.example` → `env.windows.local`.

```batch
set HISE_EXE=...
set HISE_SRC=...
set PROJECT_DIR=...
```

Use local HISE CLI (`export_ci` + `batchCompile.bat`). See HISE lane checklist in docs.

---

Cursor Cloud does **not** remote-control this IDE; sync is **git + handoff JSON + Slack on new inbox files**.
