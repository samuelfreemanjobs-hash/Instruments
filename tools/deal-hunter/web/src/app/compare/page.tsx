import { CompareClient } from "./compare-client";

export default function ComparePage() {
  return (
    <section className="space-y-4">
      <header className="space-y-1">
        <h2 className="text-lg font-semibold">Comparison center</h2>
        <p className="text-sm text-bfm-muted">
          Pricing agent compares live listings, coupons, shipping, and returns — pair with Intel for buy/wait calls.
        </p>
      </header>
      <CompareClient />
    </section>
  );
}
