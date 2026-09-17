"use client";

import { useState, useTransition } from "react";
import { useRouter } from "next/navigation";
import {
  simulateDevPaymentAction,
  startStripeCheckoutAction,
} from "@/lib/actions/payments";

declare global {
  interface Window {
    Razorpay?: new (options: Record<string, unknown>) => { open: () => void };
  }
}

function loadRazorpayScript() {
  return new Promise<boolean>((resolve) => {
    if (window.Razorpay) {
      resolve(true);
      return;
    }
    const script = document.createElement("script");
    script.src = "https://checkout.razorpay.com/v1/checkout.js";
    script.onload = () => resolve(true);
    script.onerror = () => resolve(false);
    document.body.appendChild(script);
  });
}

export function PayOrderPanel({
  orderId,
  totalUsd,
  stripeEnabled,
  razorpayEnabled,
  devSimulateEnabled,
}: {
  orderId: string;
  totalUsd: number;
  stripeEnabled: boolean;
  razorpayEnabled: boolean;
  devSimulateEnabled: boolean;
}) {
  const router = useRouter();
  const [error, setError] = useState<string | null>(null);
  const [pending, startTransition] = useTransition();

  function payStripe() {
    setError(null);
    startTransition(async () => {
      const result = await startStripeCheckoutAction(orderId);
      if (result && !result.ok) setError(result.error);
    });
  }

  function paySimulate() {
    setError(null);
    startTransition(async () => {
      const result = await simulateDevPaymentAction(orderId);
      if (result && !result.ok) setError(result.error);
    });
  }

  async function payRazorpay() {
    setError(null);
    const loaded = await loadRazorpayScript();
    if (!loaded) {
      setError("Could not load RazorPay checkout");
      return;
    }

    const res = await fetch("/api/payments/razorpay/order", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ orderId }),
    });
    const data = (await res.json()) as {
      error?: string;
      keyId?: string;
      razorpayOrderId?: string;
      amount?: number;
      currency?: string;
      shopOrderId?: string;
    };
    if (!res.ok) {
      setError(data.error ?? "RazorPay failed");
      return;
    }

    const rzp = new window.Razorpay!({
      key: data.keyId,
      amount: data.amount,
      currency: data.currency,
      name: "Cursor Shop",
      description: `Order ${orderId.slice(-8)}`,
      order_id: data.razorpayOrderId,
      handler: async (response: {
        razorpay_order_id: string;
        razorpay_payment_id: string;
        razorpay_signature: string;
      }) => {
        const verify = await fetch("/api/payments/razorpay/verify", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({
            shopOrderId: data.shopOrderId,
            razorpay_order_id: response.razorpay_order_id,
            razorpay_payment_id: response.razorpay_payment_id,
            razorpay_signature: response.razorpay_signature,
          }),
        });
        if (verify.ok) {
          router.push(`/account/orders/${orderId}?paid=1`);
          router.refresh();
        } else {
          setError("Payment verification failed");
        }
      },
      prefill: {},
      theme: { color: "#0f172a" },
    });
    rzp.open();
  }

  return (
    <div className="mt-8 space-y-3">
      {error ? <p className="text-sm text-red-600">{error}</p> : null}
      {stripeEnabled ? (
        <button
          type="button"
          disabled={pending}
          onClick={payStripe}
          className="w-full rounded-md bg-slate-900 py-2.5 text-sm font-medium text-white disabled:opacity-60"
        >
          Pay with Stripe ({totalUsd.toFixed(2)} USD)
        </button>
      ) : null}
      {razorpayEnabled ? (
        <button
          type="button"
          disabled={pending}
          onClick={() => void payRazorpay()}
          className="w-full rounded-md border border-slate-900 py-2.5 text-sm font-medium disabled:opacity-60"
        >
          Pay with RazorPay
        </button>
      ) : null}
      {devSimulateEnabled ? (
        <button
          type="button"
          disabled={pending}
          onClick={paySimulate}
          className="w-full rounded-md border border-dashed border-amber-400 py-2.5 text-sm text-amber-900"
        >
          Simulate payment (dev, no Stripe keys)
        </button>
      ) : null}
      {!stripeEnabled && !razorpayEnabled && !devSimulateEnabled ? (
        <p className="text-sm text-slate-600">
          Configure Stripe or RazorPay in `.env.local`. See `web/docs/PAYMENTS.md`.
        </p>
      ) : null}
    </div>
  );
}
