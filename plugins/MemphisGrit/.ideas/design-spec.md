# Memphis Grit — design spec (APC Design)

**UI framework:** `juce-native` (template-effect scaffold)  
**Inputs:** [creative-brief.md](creative-brief.md) · [parameter-spec.md](parameter-spec.md) · [architecture.md](architecture.md)

## Window

| Property | Value |
|----------|-------|
| Size | **520 × 340**, fixed |
| Title | MEMPHIS GRIT |
| Controls | Drive, Lo-Fi, Mid, Mix, Output (dB), Bypass |

## Layout

Header label **MEMPHIS GRIT** centered. Five rotary sliders in one row (Drive → Output). Bypass button bottom-right. Dark chassis background; accent on title (Memphis gold tone in V1 hex `#c9a227` on editor title label only).

## Parameter widgets

| Param | Widget | Range (per parameter-spec) |
|-------|--------|---------------------------|
| mg_drive | Rotary | 0–100 % |
| mg_lofi | Rotary | 0–100 % |
| mg_mid | Rotary | 0–100 % |
| mg_mix | Rotary | 0–100 % wet |
| mg_output_db | Rotary | −24 … +12 dB |
| mg_bypass | Toggle | on/off |

V1 does not use factory-lib design-system theme; follow-up SKU can adopt `disklordz` tokens.
