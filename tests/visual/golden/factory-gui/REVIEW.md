# Visual golden re-baseline review

- Generated: `2026-11-09 15:50:27Z` (UTC)
- Suite: `tests/visual/golden/factory-gui`
- Filter (`--only`): `HeaderBar*, PresetBrowser*, ValueReadout*`
- Changed goldens: **12**

Reviewer: open each before/after pair (and the HTML diff report from
`tools/visual_diff.py --out …`) before approving this PR. Do **not**
re-baseline to silence an uninspected failure (L-0005).

| Golden | Change | Before thumb | After thumb |
|--------|--------|--------------|-------------|
| `_template/HeaderBar-longest@1x.png` | pixels-changed | [before](_review/before/_template/HeaderBar-longest@1x.png) | [after](_review/after/_template/HeaderBar-longest@1x.png) |
| `_template/HeaderBar-longest@2x.png` | pixels-changed | [before](_review/before/_template/HeaderBar-longest@2x.png) | [after](_review/after/_template/HeaderBar-longest@2x.png) |
| `_template/PresetBrowser-default@1x.png` | pixels-changed | [before](_review/before/_template/PresetBrowser-default@1x.png) | [after](_review/after/_template/PresetBrowser-default@1x.png) |
| `_template/PresetBrowser-default@2x.png` | pixels-changed | [before](_review/before/_template/PresetBrowser-default@2x.png) | [after](_review/after/_template/PresetBrowser-default@2x.png) |
| `disklordz/HeaderBar-longest@1x.png` | pixels-changed | [before](_review/before/disklordz/HeaderBar-longest@1x.png) | [after](_review/after/disklordz/HeaderBar-longest@1x.png) |
| `disklordz/HeaderBar-longest@2x.png` | pixels-changed | [before](_review/before/disklordz/HeaderBar-longest@2x.png) | [after](_review/after/disklordz/HeaderBar-longest@2x.png) |
| `disklordz/PresetBrowser-default@1x.png` | pixels-changed | [before](_review/before/disklordz/PresetBrowser-default@1x.png) | [after](_review/after/disklordz/PresetBrowser-default@1x.png) |
| `disklordz/PresetBrowser-default@2x.png` | pixels-changed | [before](_review/before/disklordz/PresetBrowser-default@2x.png) | [after](_review/after/disklordz/PresetBrowser-default@2x.png) |
| `studio-light/HeaderBar-longest@1x.png` | pixels-changed | [before](_review/before/studio-light/HeaderBar-longest@1x.png) | [after](_review/after/studio-light/HeaderBar-longest@1x.png) |
| `studio-light/HeaderBar-longest@2x.png` | pixels-changed | [before](_review/before/studio-light/HeaderBar-longest@2x.png) | [after](_review/after/studio-light/HeaderBar-longest@2x.png) |
| `studio-light/PresetBrowser-default@1x.png` | pixels-changed | [before](_review/before/studio-light/PresetBrowser-default@1x.png) | [after](_review/after/studio-light/PresetBrowser-default@1x.png) |
| `studio-light/PresetBrowser-default@2x.png` | pixels-changed | [before](_review/before/studio-light/PresetBrowser-default@2x.png) | [after](_review/after/studio-light/PresetBrowser-default@2x.png) |

## Paths

- Thumbnails live under `_review/before/` and `_review/after/`.
- Sidecar JSON changes are listed when the paired `.png` changed or the JSON digest differs.
