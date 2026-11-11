import { LoginForm } from "./login-form";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export default function LoginPage() {
  return (
    <div className="mx-auto max-w-md space-y-4">
      <h2 className="text-lg font-semibold">Sign in</h2>
      <p className="text-sm text-bfm-muted">
        Magic link via Supabase Auth. Wishlists, alerts, and budgets sync when RLS tables are live.
      </p>
      {!isSupabaseConfigured() ? (
        <p className="rounded-lg border border-bfm-border bg-bfm-card p-4 text-sm">
          Bind <code>SUPABASE_URL</code> and <code>SUPABASE_ANON_KEY</code> in the published app Secrets (or{" "}
          <code>web/.env.local</code> for local dev).
        </p>
      ) : (
        <LoginForm />
      )}
    </div>
  );
}
