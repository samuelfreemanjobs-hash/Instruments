"use client";

import Link from "next/link";
import { X } from "lucide-react";
import { useCart } from "@/components/store/CartContext";
import { formatPrice } from "@/lib/utils";

export function FavoritesDrawer() {
  const { favorites, favoritesOpen, setFavoritesOpen, addToCart, toggleFavorite } = useCart();

  if (!favoritesOpen) return null;

  return (
    <div className="fixed inset-0 z-50 flex justify-end">
      <button
        type="button"
        className="absolute inset-0 bg-black/40"
        aria-label="Close favorites"
        onClick={() => setFavoritesOpen(false)}
      />
      <aside className="relative flex h-full w-full max-w-md flex-col bg-white shadow-xl">
        <div className="flex items-center justify-between border-b px-4 py-4">
          <h2 className="text-lg font-semibold">Favorites</h2>
          <button type="button" onClick={() => setFavoritesOpen(false)}>
            <X className="h-5 w-5" />
          </button>
        </div>
        <div className="flex-1 overflow-y-auto p-4">
          {favorites.length === 0 ? (
            <p className="text-sm text-slate-600">Save products with the heart icon.</p>
          ) : (
            <ul className="space-y-4">
              {favorites.map((p) => (
                <li key={p.id} className="flex items-center justify-between gap-2 border-b pb-3">
                  <div>
                    <Link
                      href={`/product/${p.slug}`}
                      className="font-medium hover:underline"
                      onClick={() => setFavoritesOpen(false)}
                    >
                      {p.title}
                    </Link>
                    <p className="text-sm text-slate-600">{formatPrice(p.price)}</p>
                  </div>
                  <div className="flex flex-col gap-1">
                    <button
                      type="button"
                      className="text-xs text-slate-900 underline"
                      onClick={() => void addToCart(p.id)}
                    >
                      Add to cart
                    </button>
                    <button
                      type="button"
                      className="text-xs text-red-600"
                      onClick={() => void toggleFavorite(p.id)}
                    >
                      Remove
                    </button>
                  </div>
                </li>
              ))}
            </ul>
          )}
        </div>
      </aside>
    </div>
  );
}
