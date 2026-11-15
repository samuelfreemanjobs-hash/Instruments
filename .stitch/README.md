# Stitch Canvas — JD Upgraded

| Field | Value |
|-------|--------|
| **Project ID** | `4458054159206462354` |
| **Reference screen** | [`0cd8c1ef7e7d45289902c2bdf827b923`](https://stitch.google.com/projects/4458054159206462354/screens/0cd8c1ef7e7d45289902c2bdf827b923) |
| **Style guide** | [`docs/JD_UPGRADED_V_COLLECTION_STYLE_GUIDE.md`](../docs/JD_UPGRADED_V_COLLECTION_STYLE_GUIDE.md) |
| **Canvas** | [Open in Stitch](https://stitch.google.com/projects/4458054159206462354) |
| **Design spec** | [`DESIGN.md`](../.stitch/DESIGN.md) (lint/upload); repo index [`DESIGN.md`](../DESIGN.md) |

Local repo pins the project via `.stitch.json` (`stitch config set project …`) and this manifest.

## Sam / agents (after `stitch login` or `STITCH_API_KEY`)

```bash
stitch upload design .stitch/DESIGN.md
stitch find screens --project 4458054159206462354 --json
stitch status --flow=design --json
```

Export layers from Canvas → `shared/disklordz_ui/skins/jdupgraded_default/assets/` per `docs/UI_DESIGN_HANDOFF.md`.

Credentials are **not** committed.