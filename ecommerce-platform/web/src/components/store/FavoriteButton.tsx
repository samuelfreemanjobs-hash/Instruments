"use client";

import { Heart } from "lucide-react";
import { useCart } from "@/components/store/CartContext";
import { cn } from "@/lib/utils";

export function FavoriteButton({ productId }: { productId: string }) {
  const { isFavorite, toggleFavorite } = useCart();
  const active = isFavorite(productId);

  return (
    <button
      type="button"
      aria-label={active ? "Remove from favorites" : "Add to favorites"}
      onClick={() => void toggleFavorite(productId)}
      className={cn(
        "inline-flex items-center gap-2 rounded-md border px-4 py-2 text-sm",
        active ? "border-red-200 bg-red-50 text-red-700" : "border-slate-200",
      )}
    >
      <Heart className={cn("h-4 w-4", active && "fill-current")} />
      {active ? "Saved" : "Save"}
    </button>
  );
}
