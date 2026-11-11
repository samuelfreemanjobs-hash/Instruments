# SOP-VF-08 — Release & ship

## Purpose

Turn a certified build into a versioned, installable package under `release/<Name>/` with notes and evidence.

## Preconditions

- Factory certification passed; `approve_release` approved.
- `bash tools/apc-gate.sh <Name> ship` exits 0 (needs a built `.vst3`).

## Procedure

1. **Version:** semver in `vst/<Name>/CMakeLists.txt` `project(... VERSION x.y.z)` and `status.json → version`. Patch = fixes only; minor = new features/parameters (append-only ids); major = breaking state/parameter changes.
2. **Release notes:** `bin/apc template <Name> release --version x.y.z` → `plugins/<Name>/RELEASE_NOTES.md`; fill in what's new, fixes, known issues, compatibility, verification evidence (paste command results).
3. **Package:** `tools/package-vst.sh <Name> <version>` (or factory `POST /api/projects/{id}/ship`) → `release/<Name>/<Name>-<version>-vst3.zip` containing the bundle and `INSTALL.txt`. The version argument defaults to 1.0.0, so always pass it.
4. **Fonts and licences:** confirm the zip includes the theme font licences (OFL texts) and the repo licence notice.
5. **Smoke the zip:** unzip into a scratch folder (never the system VST3 folder from an agent), check the bundle and INSTALL.txt.
6. **Status:** `ship_ready` true, `current_phase` `ship`, `phase_history` entry.
7. **PR:** release PR with notes + evidence; merge after CI is green and the operator approves.

## Definition of done

- Zip exists under `release/<Name>/`, its name matches the version, INSTALL.txt present.
- `RELEASE_NOTES.md` has no template comments left.

## Related

[.agents/skills/ship/SKILL.md](../../../.agents/skills/ship/SKILL.md) · [tools/package-vst.sh](../../../tools/package-vst.sh) · [SOP-VF-06](SOP-VF-06-verify-test.md)
