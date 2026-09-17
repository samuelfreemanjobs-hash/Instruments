"use client";

import { useState } from "react";
import { useCart } from "@/components/store/CartContext";

export function AddToCartButton({ productId, disabled }: { productId: string; disabled?: boolean }) {
  const { addToCart } = useCart();
  const [pending, setPending] = useState(false);

  return (
    <button
      type="button"
      disabled={disabled || pending}
      onClick={async () => {
        setPending(true);
        await addToCart(productId, 1);
        setPending(false);
      }}
      className="mt-8 rounded-md bg-slate-900 px-6 py-3 text-sm font-medium text-white disabled:opacity-50"
    >
      {pending ? "Adding…" : "Add to cart"}
    </button>
  );
}
