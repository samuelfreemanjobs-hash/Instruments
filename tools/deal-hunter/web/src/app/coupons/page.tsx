import { CouponsClient } from "./coupons-client";

export default function CouponsPage() {
  return (
    <section className="space-y-4">
      <header className="space-y-1">
        <h2 className="text-lg font-semibold">Coupon center</h2>
        <p className="text-sm text-bfm-muted">Verified codes only — confirm at checkout; see Final Price on each deal.</p>
      </header>
      <CouponsClient />
    </section>
  );
}
