---
name: release-package
description: Packages a plugin or drum kit for release. Use when the user says release, ship, notarize, or cut a version.
---

# Release package

Stop if any of these are false:

- Version in `CMakeLists.txt` matches the changelog entry.
- `DrumEngineTests` or the plugin's own tests passed.
- For a VST3, pluginval passed, or the log shows an infrastructure 504 rather than a failed test.
- `ip-guard` found no live secrets or absolute sample paths.

Linux can prepare the tree and the checksums. It cannot run `codesign`, `notarytool`, or `signtool`.

```bash
bash disklordz/drum-engine/packaging/macos/notarize.sh   # exits 2 on Linux with the reason
# Windows installer script (edit, do not run ISCC here):
# disklordz/drum-engine/packaging/windows/DrumEngine.iss
python3 disklordz/drum-engine/python/process_kit.py RAW --out OUT --zip
sha256sum OUT.zip
```

macOS, on a Mac, after the VST3 exists: sign with Developer ID, `xcrun notarytool submit --wait`, `xcrun stapler staple`, then `spctl --assess`. Windows: build the Inno script and Authenticode-sign the installer.

Do not tag, push a tag, or upload a binary unless the user says go.

Write a one-line status per stage. End with the artifact path, the checksum, and the human step still left (distributor upload, website version, or notarization on a Mac).
