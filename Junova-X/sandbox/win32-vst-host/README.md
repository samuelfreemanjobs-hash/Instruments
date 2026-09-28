# Win32 VST sandbox (legacy Juno reference plugins)

Isolated **Wine win32** prefix for agent/local use with **32-bit Windows VST2** instruments (Yonu60, RJU-60, Sixth Month June, old TAL builds). Outputs go to `renders/` for comparison with Junova via `compare_jun6_reference.sh`.

**Not in CI** — requires Wine, optional GUI host binary, and **you-supplied** plugin DLLs (we do not redistribute them).

## Layout

```text
Junova-X/sandbox/win32-vst-host/
├── README.md                 ← this file
├── ARCHITECTURE.md
├── setup.sh                  ← Wine win32 prefix + deps
├── run_sandbox.sh            ← firejail + xvfb wrapper
├── run_vsthost.sh            ← start Hermann Seib VSTHost under Wine
├── Dockerfile                ← optional full image
├── .gitignore
├── wineprefix/               ← WINEPREFIX (gitignored)
├── host/                     ← drop VSTHost.exe here (gitignored binaries)
├── plugins/                  ← drop *.dll VST2 here (gitignored)
├── midi/                     ← test MIDI (repo can ship small .mid)
└── renders/                  ← WAV output (gitignored)
```

## Quick start

```bash
cd Junova-X/sandbox/win32-vst-host
./setup.sh

# Obtain VSTHost (donationware) from Hermann Seib and copy:
#   host/VSTHost.exe
# Copy plugin DLLs, e.g.:
#   plugins/Yonu60.dll
#   plugins/RJU-60.dll

./run_vsthost.sh
```

Headless (virtual display):

```bash
./run_sandbox.sh ./run_vsthost.sh
```

Record a reference WAV inside VSTHost (Engine → Wave Recorder) or use its MIDI player, then:

```bash
./tests/golden/junova/compare_jun6_reference.sh \
  Junova-X/sandbox/win32-vst-host/renders/my-capture.wav ab03-chorus-i 57 3.0
```

## Automation limits

VSTHost is **GUI-first** (MIDI player + wave recorder). There is no stable CLI render-to-WAV in this sandbox yet. For **headless** reference renders prefer:

- **Ultramaster KR-106** — `Junova-X/scripts/setup_kr106_reference.sh` + `render_midi`
- **64-bit** free plugins (EightySix, TAL-Chorus-LX native Linux build)

Future WO: saved VSTHost “Performance” + scripted keystrokes, or a tiny win32 CLI host added to `host/`.

## Security

`run_sandbox.sh` uses **firejail** when available (`--net=none`, `--private-tmp`, whitelist only this tree + Junova build artefacts). Untrusted DLLs stay in `plugins/` — treat as malware-capable; never run outside the sandbox.

## Related

- [FREE_JUNO_VST_CATALOG.md](../../docs/FREE_JUNO_VST_CATALOG.md)
- [REFERENCE_PLUGINS.md](../../docs/REFERENCE_PLUGINS.md)
