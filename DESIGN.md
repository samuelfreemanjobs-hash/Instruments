# Design system entry point

Canonical spec for Stitch and `@google/design.md` lives at **[.stitch/DESIGN.md](.stitch/DESIGN.md)** (Anodized Studio Rack VST3).

```bash
npx -p @google/design.md designmd lint .stitch/DESIGN.md
stitch upload design .stitch/DESIGN.md   # after stitch login
```

Agents: read `.stitch/DESIGN.md` before any UI, WebView, or skin work. See [docs/JUCE_STITCH_GUI_REFERENCE.md](docs/JUCE_STITCH_GUI_REFERENCE.md).