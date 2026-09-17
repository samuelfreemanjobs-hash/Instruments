"use server";

import { revalidatePath } from "next/cache";
import { readFavoriteIds, writeFavoriteIds } from "@/lib/cart/cookies";

export async function toggleFavoriteAction(productId: string) {
  const ids = await readFavoriteIds();
  const exists = ids.includes(productId);
  const next = exists ? ids.filter((id) => id !== productId) : [...ids, productId];
  await writeFavoriteIds(next);
  revalidatePath("/", "layout");
  return { ok: true as const, favorited: !exists };
}
