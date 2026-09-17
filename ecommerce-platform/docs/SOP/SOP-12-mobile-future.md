# SOP-12 — Future mobile app (Cursor / Trae)

## Strategy

Reuse existing Next.js **Route Handlers** as JSON APIs for React Native / Expo app.

## Steps (when started)

1. Document endpoints in `docs/api-contracts.md` (generate from templates).
2. Add token-based auth variant alongside cookie sessions.
3. Share Zod schemas via `web/src/lib/validators` package export.

## Definition of Done

- [ ] API contract doc exists
- [ ] Mobile app consumes `/api/products`, `/api/cart`, `/api/orders` (to be implemented)

## Verification

Postman collection or OpenAPI lint.
