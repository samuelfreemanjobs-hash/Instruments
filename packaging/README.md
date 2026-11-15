# Packaging (Ship phase)

Default path: **`tools/package-vst.sh <PluginName> [version]`**

Produces:

- `release/<Name>/<Name>-<version>-vst3.zip`
- `INSTALL.txt` + `manifest.json` inside the zip

## Professional installers (optional)

| OS | Tool | Notes |
|----|------|--------|
| Windows | [Inno Setup](https://jrsoftware.org/isinfo.php) | Script template TBD under `packaging/windows/` |
| macOS | `pkgbuild` / Packages.app | AU/VST3 codesigning required for distribution |
| Linux | zip + `INSTALL.txt` | Default in `package-vst.sh` |

Run **`/apc-ship <Name>`** after **`/apc-test`** — agents must not skip Ship for “finished” plugins.
