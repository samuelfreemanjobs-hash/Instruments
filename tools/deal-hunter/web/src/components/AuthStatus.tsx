import Link from "next/link";

import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export async function AuthStatus() {
  if (!isSupabaseConfigured()) {
    return (
      <p className="text-xs text-bfm-muted">
        Supabase not configured — set <code className="text-bfm-accent">.env.local</code> (see{" "}
        <code>.env.example</code>).
      </p>
    );
  }

  const supabase = await createClient();
  const { data } = await supabase!.auth.getUser();
  const user = data.user;

  if (!user) {
    return (
      <Link
        href="/login"
        className="inline-block self-start rounded-full bg-bfm-accent px-5 py-2.5 text-base font-semibold text-white"
      >
        Sign in
      </Link>
    );
  }

  return (
    <p className="text-xs text-bfm-muted">
      Signed in as <span className="text-white">{user.email}</span>
    </p>
  );
}
