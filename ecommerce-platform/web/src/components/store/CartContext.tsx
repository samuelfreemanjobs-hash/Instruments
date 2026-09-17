"use client";

import { createContext, useContext, useState, useCallback } from "react";
import { useRouter } from "next/navigation";
import type { CartSnapshot } from "@/lib/cart/types";
import type { FavoriteProduct } from "@/lib/cart/server";
import {
  addToCartAction,
  removeFromCartAction,
  updateCartQuantityAction,
} from "@/lib/actions/cart";
import { toggleFavoriteAction } from "@/lib/actions/favorites";

type CartContextValue = {
  cart: CartSnapshot;
  favorites: FavoriteProduct[];
  cartOpen: boolean;
  favoritesOpen: boolean;
  setCartOpen: (v: boolean) => void;
  setFavoritesOpen: (v: boolean) => void;
  addToCart: (productId: string, qty?: number) => Promise<void>;
  updateQty: (productId: string, qty: number) => Promise<void>;
  removeLine: (productId: string) => Promise<void>;
  toggleFavorite: (productId: string) => Promise<void>;
  isFavorite: (productId: string) => boolean;
};

const CartContext = createContext<CartContextValue | null>(null);

export function CartProvider({
  initialCart,
  initialFavorites,
  children,
}: {
  initialCart: CartSnapshot;
  initialFavorites: FavoriteProduct[];
  children: React.ReactNode;
}) {
  const router = useRouter();
  const [cartOpen, setCartOpen] = useState(false);
  const [favoritesOpen, setFavoritesOpen] = useState(false);

  const refresh = useCallback(() => router.refresh(), [router]);

  const addToCart = async (productId: string, qty = 1) => {
    await addToCartAction(productId, qty);
    setCartOpen(true);
    refresh();
  };

  const updateQty = async (productId: string, qty: number) => {
    await updateCartQuantityAction(productId, qty);
    refresh();
  };

  const removeLine = async (productId: string) => {
    await removeFromCartAction(productId);
    refresh();
  };

  const toggleFavorite = async (productId: string) => {
    await toggleFavoriteAction(productId);
    refresh();
  };

  const isFavorite = (productId: string) => initialFavorites.some((p) => p.id === productId);

  return (
    <CartContext.Provider
      value={{
        cart: initialCart,
        favorites: initialFavorites,
        cartOpen,
        favoritesOpen,
        setCartOpen,
        setFavoritesOpen,
        addToCart,
        updateQty,
        removeLine,
        toggleFavorite,
        isFavorite,
      }}
    >
      {children}
    </CartContext.Provider>
  );
}

export function useCart() {
  const ctx = useContext(CartContext);
  if (!ctx) throw new Error("useCart must be used within CartProvider");
  return ctx;
}
