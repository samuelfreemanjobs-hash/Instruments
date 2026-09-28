# Disklordz Juno106 iPlug2 product (import slot)

The **canonical ship tree** is JUCE [`Junova-X/`](../../../Junova-X/REPO_HANDOFF.md).

Place the private iPlug2 project here when importing:

```text
plugin/Juno106/
├── Juno106.sln / .xcodeproj
├── config.h
├── *DSP* / *UI*
└── dependencies/   ← from plugin/scripts/fetch-deps.sh (VST3 SDK)
```

Framework submodule: [`../../third_party/iPlug2`](../../third_party/iPlug2).

Checklist: [Junova-X/docs/IPLUG2_REFERENCE.md](../../../Junova-X/docs/IPLUG2_REFERENCE.md).
