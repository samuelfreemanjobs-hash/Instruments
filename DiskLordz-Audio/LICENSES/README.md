# Third-party licenses (DiskLordz Audio)

Maintain notices for every SDK/library linked into shipping plugins. Update before each commercial release.

| Component | License | Notes |
|-----------|---------|--------|
| **JUCE** | GPL / Commercial or JUCE **Starter** | Revenue cap until paid tier; track in `license-compliance` agent |
| **VST3 SDK** | MIT (Steinberg 3.8+) | Keep SDK EULA + MIT notice in release packages |
| **CMake / toolchain** | BSD / respective | Document versions in release notes |
| **Product DSP/MIDI code** | Proprietary (DiskLordz Audio) | Copyright DiskLordz Audio |

## Release checklist

- [ ] `LICENSES/` copied into customer ZIP
- [ ] JUCE module list matches linked binaries
- [ ] No Steinberg/VST trademarks misused in branding

See `docs/DISKLORDZ_MASTER_PLAN.md` (licensing section) and agent `license-compliance`.
