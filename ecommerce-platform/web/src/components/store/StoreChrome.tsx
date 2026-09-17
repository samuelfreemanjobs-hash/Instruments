"use client";

import Link from "next/link";
import { Heart, ShoppingBag } from "lucide-react";
import type { SessionPayload } from "@/lib/auth/session";
import type { CartSnapshot } from "@/lib/cart/types";
import type { FavoriteProduct } from "@/lib/cart/server";
import { AccountNav } from "@/components/store/AccountNav";
import { CartProvider, useCart } from "@/components/store/CartContext";
import { CartDrawer } from "@/components/store/CartDrawer";
import { FavoritesDrawer } from "@/components/store/FavoritesDrawer";

function HeaderNav({ session }: { session: SessionPayload | null }) {
  const { cart, favorites, setCartOpen, setFavoritesOpen } = useCart();

  return (
    <header className="border-b border-slate-200">
      <div className="mx-auto flex max-w-6xl items-center justify-between gap-4 px-4 py-4">
        <Link href="/" className="text-lg font-semibold tracking-tight">
          Cursor Shop
        </Link>
        <nav className="flex items-center gap-4 text-sm md:gap-6">
          <Link href="/shop" className="hover:text-slate-600">
            Shop
          </Link>
          <AccountNav session={session} />
          <button
            type="button"
            onClick={() => setFavoritesOpen(true)}
            className="relative flex items-center gap-1 hover:text-slate-600"
          >
            <Heart className="h-4 w-4" />
            {favorites.length > 0 ? (
              <span className="absolute -right-2 -top-2 flex h-4 min-w-4 items-center justify-center rounded-full bg-slate-900 px-1 text-[10px] text-white">
                {favorites.length}
              </span>
            ) : null}
            <span className="hidden sm:inline">Saved</span>
          </button>
          <button
            type="button"
            onClick={() => setCartOpen(true)}
            className="relative flex items-center gap-1 hover:text-slate-600"
          >
            <ShoppingBag className="h-4 w-4" />
            {cart.itemCount > 0 ? (
              <span className="absolute -right-2 -top-2 flex h-4 min-w-4 items-center justify-center rounded-full bg-slate-900 px-1 text-[10px] text-white">
                {cart.itemCount}
              </span>
            ) : null}
            Cart
          </button>
          {session?.role === "ADMIN" ? (
            <Link href="/admin" className="text-slate-500 hover:text-slate-800">
              Admin
            </Link>
          ) : null}
        </nav>
      </div>
    </header>
  );
}

export function StoreChrome({
  session,
  cart,
  favorites,
  children,
}: {
  session: SessionPayload | null;
  cart: CartSnapshot;
  favorites: FavoriteProduct[];
  children: React.ReactNode;
}) {
  return (
    <CartProvider initialCart={cart} initialFavorites={favorites}>
      <HeaderNav session={session} />
      <CartDrawer />
      <FavoritesDrawer />
      <main>{children}</main>
    </CartProvider>
  );
}
