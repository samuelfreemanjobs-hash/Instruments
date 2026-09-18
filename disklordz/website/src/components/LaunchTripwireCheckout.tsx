"use client";

import { useState } from "react";

export function LaunchTripwireCheckout({ defaultEmail = "" }: { defaultEmail?: string }) {
  const [email, setEmail] = useState(defaultEmail);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const checkout = async () => {
    setLoading(true);
    setError(null);
    try {
      const res = await fetch("/api/launch/checkout", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ email: email || undefined }),
      });
      const data = (await res.json()) as { url?: string; error?: string };
      if (!res.ok || !data.url) {
        setError(data.error ?? "Checkout unavailable — PM must set STRIPE_TRIPWIRE_PRICE_ID.");
        return;
      }
      window.location.href = data.url;
    } catch {
      setError("Network error.");
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="mt-4 rounded-xl border border-zinc-700 bg-zinc-950/60 p-4">
      <p className="text-sm font-medium text-zinc-200">Memphis Vault Lite — Stripe checkout</p>
      <label className="mt-2 block text-xs text-zinc-400">
        Email (optional, prefill receipt)
        <input
          type="email"
          value={email}
          onChange={(e) => setEmail(e.target.value)}
          className="mt-1 w-full rounded border border-zinc-700 bg-zinc-900 px-3 py-2 text-sm text-zinc-100"
        />
      </label>
      {error && <p className="mt-2 text-xs text-red-400">{error}</p>}
      <button
        type="button"
        disabled={loading}
        onClick={() => void checkout()}
        className="mt-3 w-full rounded-lg bg-amber-500 py-2.5 text-sm font-semibold text-zinc-950 hover:bg-amber-400 disabled:opacity-60"
      >
        {loading ? "Redirecting…" : "Buy tripwire sample"}
      </button>
    </div>
  );
}
