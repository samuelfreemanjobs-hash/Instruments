"use server";

import { revalidatePath } from "next/cache";

import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export async function createWishlist(name: string) {
  if (!isSupabaseConfigured()) return { ok: false as const, error: "supabase_not_configured" };
  const supabase = await createClient();
  const { data: userData } = await supabase!.auth.getUser();
  if (!userData.user) return { ok: false as const, error: "not_signed_in" };

  const { data, error } = await supabase!
    .from("wishlists")
    .insert({ user_id: userData.user.id, name: name.slice(0, 80) })
    .select("id")
    .single();

  if (error) return { ok: false as const, error: error.message };
  revalidatePath("/wishlist");
  return { ok: true as const, id: data.id as string };
}

export async function deleteWishlist(wishlistId: string) {
  if (!isSupabaseConfigured()) return { ok: false as const, error: "supabase_not_configured" };
  const supabase = await createClient();
  const { error } = await supabase!.from("wishlists").delete().eq("id", wishlistId);
  if (error) return { ok: false as const, error: error.message };
  revalidatePath("/wishlist");
  return { ok: true as const };
}

export async function addWishlistItem(
  wishlistId: string,
  externalDealId: string,
  targetPrice?: number,
  note?: string,
) {
  if (!isSupabaseConfigured()) return { ok: false as const, error: "supabase_not_configured" };
  const supabase = await createClient();
  const { error } = await supabase!.from("wishlist_items").insert({
    wishlist_id: wishlistId,
    external_deal_id: externalDealId,
    target_price: targetPrice ?? null,
    note: note?.slice(0, 200) ?? null,
  });
  if (error) return { ok: false as const, error: error.message };
  revalidatePath("/wishlist");
  return { ok: true as const };
}

export async function removeWishlistItem(itemId: string) {
  if (!isSupabaseConfigured()) return { ok: false as const, error: "supabase_not_configured" };
  const supabase = await createClient();
  const { error } = await supabase!.from("wishlist_items").delete().eq("id", itemId);
  if (error) return { ok: false as const, error: error.message };
  revalidatePath("/wishlist");
  return { ok: true as const };
}

/** Ensures default list exists, then adds catalog deal by external id */
export async function addDealToWishlist(externalDealId: string, title: string, price: number) {
  if (!isSupabaseConfigured()) return { ok: false as const, error: "supabase_not_configured" };
  const supabase = await createClient();
  const { data: userData } = await supabase!.auth.getUser();
  if (!userData.user) return { ok: false as const, error: "not_signed_in" };

  let { data: lists } = await supabase!
    .from("wishlists")
    .select("id")
    .eq("user_id", userData.user.id)
    .limit(1);

  let listId = lists?.[0]?.id as string | undefined;
  if (!listId) {
    const created = await createWishlist("Saved deals");
    if (!created.ok) return created;
    listId = created.id;
  }

  return addWishlistItem(listId, externalDealId, price, title.slice(0, 200));
}
