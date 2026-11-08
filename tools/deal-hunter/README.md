# Deal Hunter PWA

ADHD-friendly deal tracker for your household: **Black Friday → Cyber Monday → holidays → groceries/home** as the year rolls on. Verified retailer links, coupon copy buttons, used/refurb filters, shared checklist list, and steal / price-drop alerts.

## Quick start

```bash
npm ci
npm run dev
```

Open http://127.0.0.1:5173 — install from the browser banner on mobile or desktop.

## Sharing a list with your partner

1. Add deals from **Find deals**.
2. Open **Our list** → **Create share code**.
3. Share the 8-character code; they enter it under **Join**.

Tap **Sync** after edits so the server copy stays current.

## Notifications

**Alerts** → set minimum steal score → **Enable notifications**. Watch individual items with **Watch price** on each card.

Deal data in this repo is a curated demo catalog. Wire your own APIs in `server/catalog.ts` for live retailer data.
