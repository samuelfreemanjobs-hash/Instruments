# Magic link email not arriving

The app calls `signInWithOtp` with `emailRedirectTo`:

`https://black-friday-madness.apps.grok.bot/auth/callback?next=/dashboard`

## Supabase dashboard (required)

**Authentication → URL Configuration**

| Field | Value |
|--------|--------|
| Site URL | `https://black-friday-madness.apps.grok.bot` |
| Redirect URLs | `https://black-friday-madness.apps.grok.bot/auth/callback` |
| Redirect URLs (wildcard) | `https://black-friday-madness.apps.grok.bot/**` |

Missing redirect URLs often cause API errors (shown on the login form after the server-action update). If the UI says “check your email” but nothing arrives, the request was accepted — delivery is on Supabase’s side.

## Delivery

1. **Authentication → Logs** — look for `user.sign_in` / mail events and errors.
2. **Spam / Promotions** — default Supabase mail is easy to filter.
3. **Rate limits** — free tier caps OTP sends per hour; wait and retry.
4. **SMTP (recommended)** — Authentication → SMTP: connect SendGrid, Resend, or similar for reliable delivery.
5. **Templates** — Authentication → Email Templates → “Magic Link” enabled.

## After click

Link lands on `/auth/callback?code=...` — session cookies are set server-side; you should redirect to `/dashboard`.

Local dev: add `http://localhost:3000/auth/callback` to Redirect URLs.
