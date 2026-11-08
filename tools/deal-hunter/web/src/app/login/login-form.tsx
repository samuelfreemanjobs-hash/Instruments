"use client";

import { useState } from "react";

import { createClient } from "@/lib/supabase/client";

export function LoginForm() {
  const [email, setEmail] = useState("");
  const [message, setMessage] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  const submit = async (e: React.FormEvent) => {
    e.preventDefault();
    setLoading(true);
    setMessage(null);
    const supabase = createClient();
    if (!supabase) {
      setMessage("Supabase client not configured.");
      setLoading(false);
      return;
    }
    const origin = window.location.origin;
    const { error } = await supabase.auth.signInWithOtp({
      email: email.trim(),
      options: { emailRedirectTo: `${origin}/auth/callback?next=/dashboard` },
    });
    setLoading(false);
    if (error) setMessage(error.message);
    else setMessage("Check your email for the sign-in link.");
  };

  return (
    <form onSubmit={(e) => void submit(e)} className="space-y-3 rounded-xl border border-bfm-border bg-bfm-card p-4">
      <label className="block text-sm">
        Email
        <input
          type="email"
          required
          value={email}
          onChange={(e) => setEmail(e.target.value)}
          className="mt-1 w-full rounded-lg border border-bfm-border bg-bfm-bg-2 px-3 py-2 text-white"
        />
      </label>
      <button
        type="submit"
        disabled={loading}
        className="w-full rounded-lg bg-bfm-accent px-4 py-2 text-sm font-medium text-white disabled:opacity-50"
      >
        {loading ? "Sending…" : "Email magic link"}
      </button>
      {message && <p className="text-sm text-bfm-muted">{message}</p>}
    </form>
  );
}
