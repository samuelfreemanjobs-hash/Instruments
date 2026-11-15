# Studio Light theme (`studio-light`) — example

**Use for:** proof that the design system is a template. Not a DiskLordz product theme. It is a white-label example showing a completely different design built from the same components and contract. 
**Personality:** vintage console: warm cream panels, burnt-orange accents, green "go" lamp, dark amber meter windows. 
**Preview:** [previews/studio-light.html](../../previews/studio-light.html)

## How it differs from `disklordz`

| Aspect | disklordz | studio-light |
|--------|-----------|--------------|
| Surfaces | Dark gunmetal | Light cream (`#E9E4D8` chassis) |
| Brand role | Purple | Burnt orange `#B4461A` |
| Active / go | Lime | Lamp green `#2E7D32` |
| Focus | Light purple | Blue `#1F5FBF` |
| Meters | Steel cyan on near-black | Amber on a dark meter window (`#1E1C19`) |
| Type | Inter + JetBrains Mono, uppercase product name | IBM Plex Sans + Plex Mono (OFL-1.1), title-case product name |
| Corners | 2–4 px | 3–8 px |
| Shadows | Strong black | Soft warm grey |

Every contract role and contrast pair is satisfied (`check_tokens.py --theme studio-light`).

## Policies

- `color.meter` only under `semantic.meter`; `color.premium` only under `semantic.premium`; `color.signal` only under `semantic.meter` and `semantic.state`.

## Changelog

| Version | Change |
|---------|--------|
| 0.1.0 | Initial example theme |
