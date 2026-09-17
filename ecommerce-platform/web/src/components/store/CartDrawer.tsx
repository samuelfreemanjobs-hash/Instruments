"use client";

import Link from "next/link";
import { X } from "lucide-react";
import { useCart } from "@/components/store/CartContext";
import { formatPrice } from "@/lib/utils";

export function CartDrawer() {
  const { cart, cartOpen, setCartOpen, updateQty, removeLine } = useCart();

  if (!cartOpen) return null;

  return (
    <div className="fixed inset-0 z-50 flex justify-end">
      <button
        type="button"
        className="absolute inset-0 bg-black/40"
        aria-label="Close cart"
        onClick={() => setCartOpen(false)}
      />
      <aside className="relative flex h-full w-full max-w-md flex-col bg-white shadow-xl">
        <div className="flex items-center justify-between border-b px-4 py-4">
          <h2 className="text-lg font-semibold">Cart ({cart.itemCount})</h2>
          <button type="button" onClick={() => setCartOpen(false)} aria-label="Close">
            <X className="h-5 w-5" />
          </button>
        </div>
        <div className="flex-1 overflow-y-auto p-4">
          {cart.lines.length === 0 ? (
            <p className="text-sm text-slate-600">Your cart is empty.</p>
          ) : (
            <ul className="space-y-4">
              {cart.lines.map((line) => (
                <li key={line.productId} className="flex gap-3 border-b border-slate-100 pb-4">
                  <div className="h-16 w-16 shrink-0 rounded bg-slate-100" />
                  <div className="flex-1">
                    <Link
                      href={`/product/${line.slug}`}
                      className="font-medium hover:underline"
                      onClick={() => setCartOpen(false)}
                    >
                      {line.title}
                    </Link>
                    <p className="text-sm text-slate-600">{formatPrice(line.price)}</p>
                    <div className="mt-2 flex items-center gap-2">
                      <input
                        type="number"
                        min={1}
                        max={line.stock}
                        value={line.quantity}
                        onChange={(e) => void updateQty(line.productId, Number(e.target.value))}
                        className="w-16 rounded border px-2 py-1 text-sm"
                      />
                      <button
                        type="button"
                        className="text-xs text-red-600"
                        onClick={() => void removeLine(line.productId)}
                      >
                        Remove
                      </button>
                    </div>
                  </div>
                  <p className="text-sm font-medium">{formatPrice(line.lineTotal)}</p>
                </li>
              ))}
            </ul>
          )}
        </div>
        <div className="border-t p-4">
          <div className="flex justify-between text-sm">
            <span>Subtotal</span>
            <span className="font-medium">{formatPrice(cart.subtotal)}</span>
          </div>
          <Link
            href="/checkout"
            onClick={() => setCartOpen(false)}
            className="mt-4 block rounded-md bg-slate-900 py-2.5 text-center text-sm font-medium text-white"
          >
            Checkout
          </Link>
        </div>
      </aside>
    </div>
  );
}
