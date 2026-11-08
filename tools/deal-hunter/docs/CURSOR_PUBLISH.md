# Publish on Cursor Apps

1. Default UI: `tools/deal-hunter/web`
2. Run locally: `cd tools/deal-hunter && npm run dev`
3. Bind secrets in Cursor (published app does not read this VM’s `.env`):
   - `DEAL_API_ORIGIN` — public deal API URL (or leave unset for UI-only demo; search shows API offline)
   - `NEXT_PUBLIC_SUPABASE_URL` / `NEXT_PUBLIC_SUPABASE_ANON_KEY` — optional auth + wishlist
4. Use **Publish** in the agent chat; visibility private/team/public is chosen at confirm time.

Without `DEAL_API_ORIGIN`, pages render with empty-state messaging (sample mode).
