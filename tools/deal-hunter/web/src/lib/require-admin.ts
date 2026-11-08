import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";
import { isAdminEmail } from "@/lib/admin";

export type AdminGateResult =
  | { ok: true; userId: string; email: string }
  | { ok: false; error: "supabase_not_configured" | "not_signed_in" | "forbidden" };

/** Caller identity check for privileged server actions (not layout-only). */
export async function requireAdminCaller(): Promise<AdminGateResult> {
  if (!isSupabaseConfigured()) {
    return { ok: false, error: "supabase_not_configured" };
  }
  const supabase = await createClient();
  const { data } = await supabase!.auth.getUser();
  if (!data.user?.email) {
    return { ok: false, error: "not_signed_in" };
  }
  if (!isAdminEmail(data.user.email)) {
    return { ok: false, error: "forbidden" };
  }
  return { ok: true, userId: data.user.id, email: data.user.email };
}
