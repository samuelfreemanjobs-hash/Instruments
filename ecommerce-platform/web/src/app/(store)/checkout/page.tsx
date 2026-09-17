export default function CheckoutPage() {
  return (
    <div className="mx-auto max-w-3xl px-4 py-12">
      <h1 className="text-2xl font-semibold">Checkout</h1>
      <p className="mt-2 text-slate-600">
        Shipping, coupons, Stripe/RazorPay — SOP-07 and SOP-08.
      </p>
      <div className="mt-8 grid gap-8 md:grid-cols-2">
        <form className="space-y-3 rounded-lg border border-slate-200 p-4">
          <h2 className="font-medium">Shipping</h2>
          <input placeholder="Full name" className="w-full rounded border px-3 py-2" />
          <input placeholder="Address" className="w-full rounded border px-3 py-2" />
          <input placeholder="City" className="w-full rounded border px-3 py-2" />
        </form>
        <div className="rounded-lg border border-slate-200 p-4">
          <h2 className="font-medium">Order summary</h2>
          <p className="mt-4 text-slate-600">Cart items appear here.</p>
          <button type="button" className="mt-6 w-full rounded-md bg-slate-900 py-2 text-white">
            Pay with Stripe
          </button>
        </div>
      </div>
    </div>
  );
}
