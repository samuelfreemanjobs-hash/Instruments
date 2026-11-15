import Link from "next/link";

import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";
import { loadNotificationPrefs } from "./actions";
import { SettingsClient } from "./settings-client";

export default async function SettingsPage() {
  if (!isSupabaseConfigured()) {
    return (
      <section className="space-y-3">
        <h2 className="text-lg font-semibold">Account &amp; preferences</h2>
        <p className="text-sm text-bfm-muted">Connect Supabase to enable profiles and notification prefs.</p>
      </section>
    );
  }

  const supabase = await createClient();
  const { data } = await supabase!.auth.getUser();
  const user = data.user;

  if (!user) {
    return (
      <p className="text-sm">
        <Link href="/login" className="text-bfm-accent">
          Sign in
        </Link>{" "}
        to manage preferences.
      </p>
    );
  }

  const { data: profile } = await supabase!
    .from("profiles")
    .select("display_name, email, created_at")
    .eq("id", user.id)
    .maybeSingle();

  const notif = await loadNotificationPrefs();

  return (
    <section className="space-y-4">
      <h2 className="text-lg font-semibold">Account &amp; preferences</h2>
      <dl className="rounded-xl border border-bfm-border bg-bfm-card p-4 text-sm">
        <h3 className="text-bfm-muted mb-2 font-medium">Profile</h3>
        <dt className="text-bfm-muted">Email</dt>
        <dd className="mb-2">{profile?.email ?? user.email}</dd>
        <dt className="text-bfm-muted">Display name</dt>
        <dd className="mb-2">{profile?.display_name ?? "—"}</dd>
        <dt className="text-bfm-muted">Member since</dt>
        <dd>{profile?.created_at ? new Date(profile.created_at).toLocaleDateString() : "—"}</dd>
      </dl>
      {notif.ok ? <SettingsClient initial={notif.prefs} /> : <p className="text-sm text-bfm-muted">{notif.error}</p>}
    </section>
  );
}
