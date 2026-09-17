# SOP-06 — Authentication & RBAC

## Steps

1. Implement session helpers from `templates/lib/auth-session.template.ts`.
2. Routes: `/auth/login`, `/auth/register` using `templates/storefront/auth-page.template.tsx`.
3. Hash passwords with `bcryptjs` (12 rounds).
4. Middleware: `templates/middleware/auth.middleware.template.ts` → protect `/admin/*` for `ADMIN` role.
5. Navbar account modal: login state, logout, link to profile.

## Roles

| Role | Access |
|------|--------|
| CUSTOMER | Storefront, own orders/profile |
| ADMIN | All `/admin/*` |

## Definition of Done

- [ ] Register + login works
- [ ] Customer redirected from `/admin`
- [ ] Admin can access dashboard

## Verification

Two test users (admin + customer) — document emails in seed script only, not in repo secrets.
