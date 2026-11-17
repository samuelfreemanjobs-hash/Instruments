# DR-660 references — **NOT STAGED**

**Status:** No `Boss_DR-660*.zip` (or equivalent) is present in this agent’s upload set  
(`/home/ubuntu/.cursor/projects/agent-repos-Instruments/uploads/`).

Inventoried uploads include Boss **DR-5** and **DR-55**, plus Roland **TR-808 lofi set**, but **not** DR-660.

## Proxy RE (analysis-only until true 660 WAV)

| Proxy | Why | Use for |
|-------|-----|---------|
| `../tr808/` Soft_Bd, Dist_Bd1, Hard_Bd, Dark_Bd, 808cowbl, Dirtclap, Hollowsd, Clhh, Ophh | 808-character hits Memphis producers mapped on DR machines | RATE meters, pitched-sub / cowbell language |
| `../dr5/` Kick_* / HighHat_* | Research notes DR-5 shares some drum family with 660; not a substitute ROM | Comparative duration/peak only |

Do **not** ship these PCM in the commercial pack. Meter → original synthesis only.

## When Sam uploads DR-660

1. Drop archive into uploads (or paste path).
2. Unzip under `references/dr660/samples/` (gitignored WAV).
3. Update `INVENTORY.md` + this file → `STAGED`.
4. Re-meter RATE recipe; refresh DR-660-style loops if envelopes change.
