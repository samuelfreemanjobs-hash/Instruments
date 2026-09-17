# SOP-07 — Cart, favorites, checkout

## Steps

1. **Cart drawer** — `templates/storefront/cart-drawer.template.tsx`; persist via cookie or `Cart` collection for logged-in users.
2. **Favorites** — wishlist IDs in user profile or separate collection.
3. **Checkout** — `/checkout` shipping form + order summary; apply coupon server-side.
4. **Create pending order** before redirecting to payment provider.
5. **Order pages** — `/account/orders`, `/account/orders/[id]`.

## Definition of Done

- [ ] Add/remove/update quantity in cart
- [ ] Coupon validates against rules
- [ ] Checkout creates `Order` with `PENDING` payment status

## Verification

End-to-end cart → checkout form (payment stub OK before SOP-08).
