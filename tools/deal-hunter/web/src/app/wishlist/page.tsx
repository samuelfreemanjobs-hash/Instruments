import Link from "next/link";

import { WishlistClient } from "./wishlist-client";
import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export default async function WishlistPage() {
  if (!isSupabaseConfigured()) {
    return (
      <section className="space-y-2">
        <h2 className="text-lg font-semibold">Wishlist</h2>
        <p className="text-sm text-bfm-muted">Configure Supabase to sync wishlists.</p>
      </section>
    );
  }

  const supabase = await createClient();
  const { data: userData } = await supabase!.auth.getUser();
  if (!userData.user) {
    return (
      <section className="space-y-2">
        <h2 className="text-lg font-semibold">Wishlist</h2>
        <p className="text-sm">
          <Link href="/login" className="text-bfm-accent">
            Sign in
          </Link>{" "}
          to manage lists.
        </p>
      </section>
    );
  }

  const { data: lists } = await supabase!
    .from("wishlists")
    .select("id, name, created_at")
    .eq("user_id", userData.user.id)
    .order("created_at", { ascending: false });

  const listIds = (lists ?? []).map((l) => l.id);
  const { data: items } =
    listIds.length > 0
      ? await supabase!
          .from("wishlist_items")
          .select("id, wishlist_id, external_deal_id, target_price, note, added_at")
          .in("wishlist_id", listIds)
      : { data: [] };

  return (
    <section className="space-y-3">
      <h2 className="text-lg font-semibold">Wishlist</h2>
      <WishlistClient lists={lists ?? []} items={items ?? []} />
    </section>
  );
}
