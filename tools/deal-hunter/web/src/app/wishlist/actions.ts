"use server";

import { revalidatePath } from "next/cache";

import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";
import { requireSignedInUser } from "@/lib/require-user";

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
  const gate = await requireSignedInUser();
  if (!gate.ok) return { ok: false as const, error: gate.error };
  const { data, error } = await gate.supabase
    .from("wishlists")
    .delete()
    .eq("id", wishlistId)
    .eq("user_id", gate.userId)
    .select("id")
    .maybeSingle();
  if (error) return { ok: false as const, error: error.message };
  if (!data) return { ok: false as const, error: "not_found" };
  revalidatePath("/wishlist");
  return { ok: true as const };
}

export async function addWishlistItem(
  wishlistId: string,
  externalDealId: string,
  targetPrice?: number,
  note?: string,
) {
  const gate = await requireSignedInUser();
  if (!gate.ok) return { ok: false as const, error: gate.error };
  const { data: list } = await gate.supabase
    .from("wishlists")
    .select("id")
    .eq("id", wishlistId)
    .eq("user_id", gate.userId)
    .maybeSingle();
  if (!list) return { ok: false as const, error: "forbidden" };
  const { error } = await gate.supabase.from("wishlist_items").insert({
    wishlist_id: wishlistId,
    external_deal_id: externalDealId.slice(0, 128),
    target_price: targetPrice ?? null,
    note: note?.slice(0, 200) ?? null,
  });
  if (error) return { ok: false as const, error: error.message };
  revalidatePath("/wishlist");
  return { ok: true as const };
}

export async function removeWishlistItem(itemId: string) {
  const gate = await requireSignedInUser();
  if (!gate.ok) return { ok: false as const, error: gate.error };
  const { data: item } = await gate.supabase
    .from("wishlist_items")
    .select("wishlist_id")
    .eq("id", itemId)
    .maybeSingle();
  if (!item) return { ok: false as const, error: "not_found" };
  const { data: list } = await gate.supabase
    .from("wishlists")
    .select("id")
    .eq("id", item.wishlist_id)
    .eq("user_id", gate.userId)
    .maybeSingle();
  if (!list) return { ok: false as const, error: "not_found" };
  const { error } = await gate.supabase.from("wishlist_items").delete().eq("id", itemId);
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
