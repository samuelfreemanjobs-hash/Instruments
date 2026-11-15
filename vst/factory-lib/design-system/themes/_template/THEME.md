# Starter template theme (`_template`)

The starting point for a new theme. It passes the contract as-is (neutral dark panels, blue brand, green "go", gold premium, green meters) so a new theme starts green and stays green while you edit it.

**Do not ship products on `_template`.** Create your own theme from it:

```bash
python3 vst/factory-lib/design-system/scripts/new_theme.py <id> --name "<Display name>" --brand "<Brand>"
```

Then follow [SOP-VF-03 Design-system theme](../../../../../docs/disklordz/sops/SOP-VF-03-design-system-theme.md).

## Fill in for your theme

Replace each line below in your copy of this file.

| Section | What to write |
|---------|---------------|
| Use for | Which products or brand this theme serves |
| Brand source | Link to the brand guide the palette comes from |
| Personality | Two or three words, plus one sentence (e.g. "vintage console: cream panels, warm orange accents") |
| Brand anchors | Table: brand role, hex, primitive → semantic role, UI job |
| Additions | Any colours you added beyond the brand guide and why (usually lighter tints for thin lines and focus) |
| Policies | Your `usageRestrictions` and any stricter `contrast` pairs, in words |
| Type | Sans and mono families, licences, case and tracking choices |
| Logo | Which mark appears in plugin chrome and in what form (vector path) |
| Changelog | Version table; bump per the versioning rules in the design-system ARCHITECTURE.md |

## Primitive families in this starter

`neutral`, `brand`, `accent`, `premium`, `meter`, `signal`. Rename or replace them freely: components only use `semantic.*` roles. Keep every semantic role defined (the checker lists any you remove).
