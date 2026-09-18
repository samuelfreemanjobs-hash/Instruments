# Launch shippable assets

ZIPs and LICENSE files for **launch funnel SKUs** (not generated SaaS kit temp storage).

| Path (target) | SKU | Status |
|---------------|-----|--------|
| `tripwire-sample-v1/` | Memphis Vault Lite (Day 2) | **Empty** — run `[Factory]` WO |
| `vocal-stems-v1/` | Free opt-in stems | **Empty** — host + `LAUNCH_VOCAL_STEMS_URL` |

Pack convention: folders `01_Kicks` … `04_Perc`, `LICENSE.txt`, `README.txt`.

After pack + founder + CROW QC: upload to R2 or static host; set env URLs on Vercel.
