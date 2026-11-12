import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export type SignedInUser =
  | {
      ok: true;
      userId: string;
      supabase: NonNullable<Awaited<ReturnType<typeof createClient>>>;
    }
  | { ok: false; error: "supabase_not_configured" | "not_signed_in" };

/** Identity gate for user-owned server actions (defense in depth with RLS). */
export async function requireSignedInUser(): Promise<SignedInUser> {
  if (!isSupabaseConfigured()) {
    return { ok: false, error: "supabase_not_configured" };
  }
  const supabase = await createClient();
  const { data } = await supabase!.auth.getUser();
  if (!data.user) {
    return { ok: false, error: "not_signed_in" };
  }
  return { ok: true, userId: data.user.id, supabase: supabase! };
}
