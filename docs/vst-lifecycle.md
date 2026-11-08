# VST / VSTi mandatory lifecycle

Every plugin in this repo follows the same pipeline (Noizefield APC–compatible).

## The process (in order)

```mermaid
flowchart LR
  D[Dream] --> P[Plan]
  P --> De[Design]
  De --> I[Implement DSP]
  I --> T[Test]
  T --> S[Ship package]
```

| Step | What you do | What gets created |
|------|-------------|-------------------|
| **1. Dream** | `/apc-dream MyPlugin` + describe sound & knobs | `creative-brief.md`, `parameter-spec.md` |
| **2. Plan** | `/apc-plan MyPlugin` | `architecture.md`, complexity tier, UI framework |
| **3. Design** | `/apc-design MyPlugin` | `design-spec.md` (layout, colors, labels) |
| **4. Implement** | `/apc-impl MyPlugin` | `vst/MyPlugin/` — **DSP + core logic**, then editor |
| **5. Test** | `/apc-test MyPlugin` | Release build, VST3 loads |
| **6. Ship** | `/apc-ship MyPlugin` | `release/MyPlugin/` — **professional package** (zip + install notes) |

**Rule:** Cursor agents must not skip to code without Dream + Plan + Design on disk (`plugins/MyPlugin/`).

## Folders

| Path | Phase |
|------|--------|
| `plugins/<Name>/` | Dream → Design (specs only) |
| `vst/<Name>/` | Implement → Test (JUCE code) |
| `release/<Name>/` | Ship (distributable) |

## Commands (copy-paste)

```
/apc-setup
/apc-dream MyPlugin
/apc-plan MyPlugin
/apc-design MyPlugin
/apc-impl MyPlugin
/apc-test MyPlugin
/apc-ship MyPlugin
```

CLI helpers:

```bash
bash tools/apc-gate.sh MyPlugin impl    # verify phases before coding
./tools/build-vst.sh MyPlugin
./tools/package-vst.sh MyPlugin
```

## Implement phase detail

1. `./tools/new-vst.sh MyPlugin --type synth|effect` (if `vst/` missing)
2. Program chain: `shared/dsp/`, `Source/Dsp/`, `processBlock`
3. Surface chain: APVTS, `PluginEditor` per `design-spec.md`
4. Sync `params.json` with `parameter-spec.md`

## Ship phase detail

`tools/package-vst.sh` builds (if needed), copies `.vst3` + `INSTALL.txt` + version manifest into `release/<Name>/` and creates a versioned `.zip` suitable for end users.

Platform-specific installers (Inno Setup, macOS pkg) — extend `packaging/` when you target those OSes.

## Resume

`/apc-resume MyPlugin` continues from `status.json`.
