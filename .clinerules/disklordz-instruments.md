# DiskLordz Instruments (Cline rules)

Sam's Windows laptop uses this repo at `C:\dev\Instruments`. Follow the same standards as cloud agents.

## Read first

- `AGENTS.md` (build commands, lanes, security)
- `disklordz/autonomous-coder/agent-rules.md`
- Product `ARCHITECTURE.md` for the subsystem you edit
- `docs/CLAUDE_VST_ENGINEER.md` before JUCE/C++ changes

## Windows worker

Use `.cursor/rules/windows-worker.mdc`: PowerShell, `npm.cmd`, Git Bash for `.sh` scripts, single-target CMake for VST3. Do not native-compile logue ARM targets on Windows.

## Non-negotiable

- Fix only the requested task; minimal diff.
- Keep CI green; plugin PRs need a Windows x64 VST3 artifact from GitHub Actions.
- **Never merge**, never publish/post, never touch master/golden audio without explicit task text, never commit secrets.
- Peak master ceiling **-0.5 dBFS**; respect DiskLordz naming conventions.

## APC / new VSTs

Follow Dream → Plan → Design before `vst/<Name>/` (`docs/vst-lifecycle.md`). Hardware logue units live under `src/oscillators/`, not `vst/`.