# Agent context — Instruments / DiskLordz Labs

Read [ARCHITECTURE.md](ARCHITECTURE.md) first, then the product `ARCHITECTURE.md` for the area you edit.

## Org standard: JUCE for shipping plugins

This monorepo **standardizes on JUCE** for commercial plugin targets (VST3, CLAP, standalone). **JD Upgraded** is the reference: root `CMakeLists.txt`, `FetchContent` for JUCE 8.x, `clap-juce-extensions`, zero-alloc audio callback patterns in [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

**DiskLordz SP-1200 VSTi** (in development) may have been planned on **iPlug2** in an external repo; when it lands here or in a sibling repo under the same org, **use JUCE for the plugin scaffold** unless the owner explicitly keeps a separate iPlug2 product line. The SP-1200 **DSP core is framework-agnostic C++17** — only the wrapper, parameters, and UI bind to JUCE.

Do not assume Claude Code summaries that say “iPlug2 only” override this monorepo policy.

## Products in this repo

| Area | Path |
|------|------|
| JD Upgraded synth | `Source/`, `docs/ARCHITECTURE.md` |
| Offline harness | `tools/`, golden WAV CI |
| DiskLordz Factory (catalog OS) | `disklordz-factory/` |
| SP-1200 (planned / external) | [docs/DISKLORDZ_SP1200.md](docs/DISKLORDZ_SP1200.md) |

## SP-1200 immediate engineering priority

When the **DISKLORDZ** codebase is available locally:

1. Add **`tests/test_songplayer.cpp`** and wire **`run_verify.sh`** — `SongPlayer.cpp` compiles but has no suite yet.
2. Test repeat semantics (N total), nested `begin-repeat`, sub-song depth 4 + overflow, **C16** repeat restore on return, `songEnded` vs End step.
3. Then: sequencer integration layer → JUCE processor scaffold → MixMatrix → UI.

Definition of done for that repo: **`run_verify.sh`** (all sanitizer suites green).

## DiskLordz Factory

Night shift + dashboard: `./disklordz-factory/scripts/run-factory-dev.sh`. Claude project import: [docs/CLAUDE_MCP.md](docs/CLAUDE_MCP.md).

**Slack:** Factory agents post to **#disklordz-factory** when `SLACK_FACTORY_WEBHOOK_URL` is set — see [docs/FACTORY_SLACK.md](docs/FACTORY_SLACK.md). Setup: `./scripts/setup-disklordz-integrations.sh slack-factory`.

## Common traps (SP-1200 DSP — carry into JUCE port)

- Block splitting must happen at **audio callback event offsets**, not only in a stub (~10 ms jitter otherwise).
- **C8**-class bugs: processing loop running below real-time rate.
- **C12**: sample library leak on teardown (ASan).
- **C16**: sub-song calls losing repeat state.
- MixMatrix / level multiplier (**C10**) must exist before Multilevel routing is meaningful.
