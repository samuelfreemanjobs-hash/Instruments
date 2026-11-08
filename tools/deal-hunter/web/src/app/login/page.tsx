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
          Add <code>NEXT_PUBLIC_SUPABASE_URL</code> and <code>NEXT_PUBLIC_SUPABASE_ANON_KEY</code> to{" "}
          <code>web/.env.local</code>.
        </p>
      ) : (
        <LoginForm />
      )}
    </div>
  );
}
