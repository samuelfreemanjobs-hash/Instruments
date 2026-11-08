import Link from "next/link";

import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export default async function WishlistPage() {
  if (!isSupabaseConfigured()) {
    return <p className="text-sm text-bfm-muted">Sign in with Supabase to sync wishlists.</p>;
  }

  const supabase = await createClient();
  const { data: userData } = await supabase!.auth.getUser();
  if (!userData.user) {
    return (
      <p className="text-sm">
        <Link href="/login" className="text-bfm-accent">
          Sign in
        </Link>{" "}
        to view wishlists.
      </p>
    );
  }

  const { data: lists } = await supabase!
    .from("wishlists")
    .select("id, name, created_at")
    .order("created_at", { ascending: false });

  return (
    <section className="space-y-3">
      <h2 className="text-lg font-semibold">Wishlist</h2>
      <ul className="space-y-2 text-sm">
        {(lists ?? []).map((w) => (
          <li key={w.id} className="rounded-lg border border-bfm-border bg-bfm-card px-3 py-2">
            {w.name}
          </li>
        ))}
        {(lists ?? []).length === 0 && (
          <li className="text-bfm-muted">No lists yet — create one after migration 002 is applied.</li>
        )}
      </ul>
    </section>
  );
}
