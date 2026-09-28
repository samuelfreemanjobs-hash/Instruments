# External plugin reference WAVs (optional)

Store **short** bounces from free reference plugins (KR-106, TAL, Tyrell, etc.) for manual or scripted A/B.

- **Do not commit** VST3/AU binaries.
- WAV files here are **gitignored** by default; add only when explicitly needed for CI (prefer KR-106 `render_midi` built from source).

Naming: `{plugin}-{scenario}-{note}-{seconds}s.wav` e.g.:

- `kr106-ab03-chorus-i-a3-3s.wav`
- `tal-chorus-lx-ab03-chorus-i-a3-3s.wav`
- `eightysix-ab02-fat-pad-c3-4s.wav`
- `yonu60-ab01-dry-saw-c4-2s.wav`
- `rju-60-ab03-chorus-i-a3-3s.wav`

Catalog: [Junova-X/docs/FREE_JUNO_VST_CATALOG.md](../../../Junova-X/docs/FREE_JUNO_VST_CATALOG.md).

See [Junova-X/docs/REFERENCE_PLUGINS.md](../../../Junova-X/docs/REFERENCE_PLUGINS.md).
