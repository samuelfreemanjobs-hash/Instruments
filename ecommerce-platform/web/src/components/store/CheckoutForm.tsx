"use client";

import { useState } from "react";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { checkoutSchema, type CheckoutInput } from "@/lib/validators/checkout";
import { placeOrderAction, previewCheckoutAction } from "@/lib/actions/checkout";
import { formatPrice } from "@/lib/utils";
import type { CartSnapshot } from "@/lib/cart/types";

export function CheckoutForm({
  cart,
  initialTotals,
  signedIn,
}: {
  cart: CartSnapshot;
  initialTotals: {
    subtotal: number;
    shipping: number;
    discount: number;
    total: number;
  };
  signedIn: boolean;
}) {
  const [totals, setTotals] = useState(initialTotals);
  const [couponError, setCouponError] = useState<string | null>(null);
  const [serverError, setServerError] = useState<string | null>(null);
  const {
    register,
    handleSubmit,
    getValues,
    formState: { errors, isSubmitting },
  } = useForm<CheckoutInput>({
    resolver: zodResolver(checkoutSchema),
    defaultValues: { shippingCountry: "US" },
  });

  async function applyCoupon() {
    const code = getValues("couponCode");
    const preview = await previewCheckoutAction(code);
    setTotals({
      subtotal: preview.subtotal,
      shipping: preview.shipping,
      discount: preview.discount,
      total: preview.total,
    });
    setCouponError(preview.couponError);
  }

  async function onSubmit(data: CheckoutInput) {
    setServerError(null);
    if (!signedIn) {
      setServerError("Please sign in to place an order");
      return;
    }
    const result = await placeOrderAction(data);
    if (result && !result.ok) setServerError(result.error);
  }

  if (cart.lines.length === 0) {
    return <p className="text-slate-600">Your cart is empty. Add products from the shop.</p>;
  }

  return (
    <div className="mt-8 grid gap-8 md:grid-cols-2">
      <form onSubmit={handleSubmit(onSubmit)} className="space-y-3 rounded-lg border border-slate-200 p-4">
        <h2 className="font-medium">Shipping</h2>
        {!signedIn ? (
          <p className="text-sm text-amber-800">Sign in to complete checkout.</p>
        ) : null}
        <input
          placeholder="Full name"
          {...register("shippingName")}
          className="w-full rounded border px-3 py-2"
        />
        {errors.shippingName ? (
          <p className="text-xs text-red-600">{errors.shippingName.message}</p>
        ) : null}
        <input
          placeholder="Address"
          {...register("shippingLine1")}
          className="w-full rounded border px-3 py-2"
        />
        <input placeholder="City" {...register("shippingCity")} className="w-full rounded border px-3 py-2" />
        <div className="grid grid-cols-2 gap-2">
          <input placeholder="ZIP" {...register("shippingZip")} className="rounded border px-3 py-2" />
          <input
            placeholder="Country"
            {...register("shippingCountry")}
            className="rounded border px-3 py-2"
          />
        </div>
        <div className="flex gap-2 pt-2">
          <input
            placeholder="Coupon code"
            {...register("couponCode")}
            className="flex-1 rounded border px-3 py-2"
          />
          <button type="button" onClick={() => void applyCoupon()} className="rounded border px-3 text-sm">
            Apply
          </button>
        </div>
        {couponError ? <p className="text-xs text-red-600">{couponError}</p> : null}
        {serverError ? <p className="text-sm text-red-600">{serverError}</p> : null}
        <button
          type="submit"
          disabled={isSubmitting || !signedIn}
          className="w-full rounded-md bg-slate-900 py-2 text-white disabled:opacity-50"
        >
          {isSubmitting ? "Placing order…" : "Place order (payment in SOP-08)"}
        </button>
      </form>
      <div className="rounded-lg border border-slate-200 p-4">
        <h2 className="font-medium">Order summary</h2>
        <ul className="mt-4 space-y-2 text-sm">
          {cart.lines.map((l) => (
            <li key={l.productId} className="flex justify-between">
              <span>
                {l.title} × {l.quantity}
              </span>
              <span>{formatPrice(l.lineTotal)}</span>
            </li>
          ))}
        </ul>
        <dl className="mt-4 space-y-1 border-t pt-4 text-sm">
          <div className="flex justify-between">
            <dt>Subtotal</dt>
            <dd>{formatPrice(totals.subtotal)}</dd>
          </div>
          <div className="flex justify-between">
            <dt>Shipping</dt>
            <dd>{totals.shipping === 0 ? "Free" : formatPrice(totals.shipping)}</dd>
          </div>
          {totals.discount > 0 ? (
            <div className="flex justify-between text-green-700">
              <dt>Discount</dt>
              <dd>-{formatPrice(totals.discount)}</dd>
            </div>
          ) : null}
          <div className="flex justify-between font-semibold">
            <dt>Total</dt>
            <dd>{formatPrice(totals.total)}</dd>
          </div>
        </dl>
      </div>
    </div>
  );
}
