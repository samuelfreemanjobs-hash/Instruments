# TODO — Activepieces automation (syncs to Airtable Projects)

**Airtable row:** `Disklordz — Activepieces automation (self-host)`  
**Push to PM:** `python3 scripts/sync_disklordz_pm_to_airtable.py` or GitHub workflow **Sync Disklordz PM to Airtable**

- [ ] Merge [PR #12](https://github.com/samuelfreemanjobs-hash/Instruments/pull/12) (or pull `cursor/disklordz-blueprint-tools-2702`)
- [ ] `cd infra/activepieces && cp .env.example .env` — set `DISKLORDZ_AP_ADMIN_EMAIL` + `DISKLORDZ_AP_ADMIN_PASSWORD` (optional `DISKLORDZ_SLACK_WEBHOOK`)
- [ ] `chmod +x automate.sh && ./automate.sh` (Docker on studio machine)
- [ ] Confirm `infra/activepieces/.disklordz-webhook-url` exists; run `tools/render_kit.py` once
- [ ] GitHub repo secret `AIRTABLE_API_KEY` → re-run **Sync Disklordz PM to Airtable**
- [ ] GitHub (optional) `DISKLORDZ_ACTIVEPIECES_WEBHOOK` for **Disklordz factory render** workflow
- [ ] Activepieces UI: connect Airtable (base `appUuhVQHAOv31wJ1`) for later store-sale flows
