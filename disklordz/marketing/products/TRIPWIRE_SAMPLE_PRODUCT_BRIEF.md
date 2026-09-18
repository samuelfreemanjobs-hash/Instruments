# Product brief — Day 2 tripwire sample pack

**PM `product_id` (proposed):** `DL-LAUNCH-TRIPWIRE-SAMPLE`  
**HELM launch:** Day 2 per [GTM_DECISIONS.md](../GTM_DECISIONS.md)  
**Stripe:** simple one-time Price (suggest **$9–$12** — PM confirms)

---

## What it is

**“Memphis Vault Lite”** (working title) — **15–20** curated phonk one-shots, not a full 100+ pack. Sells the **same 12-bit / Memphis lane** as Vault Vol. 1; upsell path to full Vault, Cassette Smash, or SaaS factory.

---

## Who creates it (not fully automated)

| Step | Owner | Tool / path |
|------|--------|-------------|
| 1. Generate candidates | **Cursor / Factory** | `POST /api/generate` preset **Midnight Circuit** (DL002) or batch factory script |
| 2. Curate + rename | **Founder (required)** | Pick best 15–20; reject fillers — **automation cannot approve taste** |
| 3. SP-1200 polish (optional) | **Founder or A&R** | RX1200 / resample hack per [PHONK_KIT_AI_SOP](../../docs/DISKLORDZ_PHONK_KIT_AI_SOP.md) |
| 4. Package ZIP | **Cursor** | `launch-assets/tripwire-sample-v1/` + LICENSE |
| 5. Cover | **VARA → mkt-brand** | Match Memphis Vault cassette line |
| 6. Commerce | **PM + Eng** | Stripe Product + download URL |
| 7. Go live Day 2 | **Founder trigger** | [HUMAN_TRIGGERS.md](../HUMAN_TRIGGERS.md) |

**Alternate fast path:** Subset of existing **Memphis Vault Vol. 1** WAVs (if you have files locally) — 15 hits repackaged as “Lite” — still requires **founder confirm** you may sell that slice.

---

## Folder layout

```text
tripwire-sample-v1/
  README.txt
  LICENSE.txt
  01_Kicks/     (3)
  02_808s/      (4, note key in filename)
  03_Snares_Claps/ (4)
  04_Perc/      (4–6 cowbells/rims)
```

No loops required for tripwire sample.

---

## Acceptance (PM)

- [ ] Founder audio sign-off
- [ ] ZIP on R2 or static URL for purchasers
- [ ] Stripe test checkout succeeds
- [ ] Email Day 2 copy matches product name and price
