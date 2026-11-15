# SOP-VF-12 — Windows operator machine

## Purpose

Work safely on the operator's Windows laptop (4 threads, 8 GB RAM, shared checkout, live DAW plugin folder).

## Rules

1. **Worktree per task:** `git worktree add C:\dev\Instruments-<task> -b cursor/<task> origin/<base>`. Never switch branches in `C:\dev\Instruments` (other agents share it).
2. **One heavy job at a time:** never a C++ build, `npm ci` and `next build` together.
3. **Build one target**, never the whole root project:
   ```powershell
   cmake -S vst\<Name> -B vst\<Name>\build -G "Visual Studio 17 2022" -A x64
   cmake --build vst\<Name>\build --config Release --target <Name>_VST3 -- /v:minimal *> vst\<Name>\build\build.log
   ```
4. **Copy-after-build off** (it writes to `C:\Program Files\Common Files\VST3`, the live DAW folder). To test in a DAW, the operator copies the `.vst3` by hand.
5. **Shell:** PowerShell; run repo `.sh` scripts with Git Bash (`bash ./tools/<script>.sh`); `npm.cmd` / `npx.cmd`; `python` = 3.12.
6. **Env vars** last only for that PowerShell window: `$env:NAME = 'value'` (not `export`).
7. **No writes** outside `C:\dev`, no global installs, no registry changes.
8. **Not available here:** g++-12, make, ninja, ARM toolchain. logue/NTS-1 builds and Linux factory QA run in CI or Cloud agents.

## pluginval / DAW smoke (tier A)

Operator-only, when online: run pluginval on the built `.vst3` from the worktree's build folder, then a DAW load/automation/save-recall smoke. Paste results into the release PR (SOP-VF-08).

## Related

[.cursor/rules/windows-worker.mdc](../../../.cursor/rules/windows-worker.mdc) · [SOP-VF-07](SOP-VF-07-factory-operations.md)
