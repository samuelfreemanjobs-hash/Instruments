import Link from "next/link";

import { LoginForm } from "@/components/LoginForm";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export default function LoginPage() {
  const configured = isSupabaseConfigured();

  return (
    <div className="min-h-screen bg-zinc-950 px-4 py-12 text-zinc-100">
      <div className="mx-auto max-w-md space-y-6">
        <h1 className="text-2xl font-semibold">Sign in</h1>
        <p className="text-sm text-zinc-400">
          Magic link to your email — save generated kits to your account.
        </p>
        {configured ? (
          <LoginForm />
        ) : (
          <p className="rounded-xl border border-amber-900/50 bg-amber-950/30 px-4 py-3 text-sm text-amber-100">
            Supabase is not configured. Set{" "}
            <code className="text-amber-200">NEXT_PUBLIC_SUPABASE_URL</code> and{" "}
            <code className="text-amber-200">NEXT_PUBLIC_SUPABASE_ANON_KEY</code>{" "}
            (see <code className="text-amber-200">.env.example</code>).
          </p>
        )}
        <Link href="/" className="text-sm text-emerald-400 hover:text-emerald-300">
          ← Back to generator
        </Link>
      </div>
    </div>
  );
}
