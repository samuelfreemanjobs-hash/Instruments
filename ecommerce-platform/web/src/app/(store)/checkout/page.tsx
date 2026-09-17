import Link from "next/link";
import { getSession } from "@/lib/auth/session";
import { getCartSnapshot, computeShipping } from "@/lib/cart/server";
import { CheckoutForm } from "@/components/store/CheckoutForm";

export const dynamic = "force-dynamic";

export default async function CheckoutPage() {
  const [session, cart] = await Promise.all([getSession(), getCartSnapshot()]);
  const shipping = computeShipping(cart.subtotal);
  const total = cart.subtotal + shipping;

  return (
    <div className="mx-auto max-w-3xl px-4 py-12">
      <h1 className="text-2xl font-semibold">Checkout</h1>
      <p className="mt-2 text-slate-600">
        Free shipping on orders over $50. Try coupon <strong>WELCOME10</strong> (min $25).
      </p>
      {!session ? (
        <p className="mt-4 text-sm">
          <Link href="/auth/login?next=/checkout" className="underline">
            Sign in
          </Link>{" "}
          to place your order.
        </p>
      ) : null}
      <CheckoutForm
        cart={cart}
        signedIn={!!session}
        initialTotals={{ subtotal: cart.subtotal, shipping, discount: 0, total }}
      />
    </div>
  );
}
