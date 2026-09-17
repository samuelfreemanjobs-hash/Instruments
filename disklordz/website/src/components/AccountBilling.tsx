"use client";

import { useState } from "react";

type Props = {
  plan: "free" | "pro";
  creditsBalance: number;
  stripeEnabled: boolean;
};

export function AccountBilling({ plan, creditsBalance, stripeEnabled }: Props) {
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const startCheckout = async () => {
    setLoading(true);
    setError(null);
    try {
      const res = await fetch("/api/stripe/checkout", { method: "POST" });
      const data = await res.json();
      if (!res.ok) {
        setError(data.message ?? data.error ?? "Checkout failed");
        return;
      }
      if (data.url) {
        window.location.href = data.url;
      }
    } catch {
      setError("Checkout failed.");
    } finally {
      setLoading(false);
    }
  };

  return (
    <section className="rounded-2xl border border-zinc-800 bg-zinc-900/50 p-5 space-y-3">
      <h2 className="text-lg font-semibold text-zinc-100">Billing</h2>
      {plan === "pro" ? (
        <p className="text-sm text-emerald-400">Pro — unlimited generations</p>
      ) : (
        <p className="text-sm text-zinc-400">
          Free plan — <span className="font-mono text-zinc-200">{creditsBalance}</span> generation
          credits remaining (1 credit = 1 batch).
        </p>
      )}
      {plan !== "pro" && stripeEnabled && (
        <button
          type="button"
          disabled={loading}
          onClick={startCheckout}
          className="rounded-xl bg-emerald-500 px-4 py-2 text-sm font-semibold text-zinc-950 hover:bg-emerald-400 disabled:opacity-50"
        >
          {loading ? "Redirecting…" : "Upgrade to Pro"}
        </button>
      )}
      {plan !== "pro" && !stripeEnabled && (
        <p className="text-xs text-zinc-500">
          Stripe not configured — set STRIPE_SECRET_KEY and STRIPE_PRO_PRICE_ID on the server.
        </p>
      )}
      {error && <p className="text-sm text-red-300">{error}</p>}
    </section>
  );
}
