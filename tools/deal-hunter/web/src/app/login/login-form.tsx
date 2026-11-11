"use client";

import { useRouter } from "next/navigation";
import { useState } from "react";

import { createClient } from "@/lib/supabase/client";

type Mode = "sign-in" | "sign-up";

const MIN_PASSWORD_LENGTH = 6;

export function LoginForm() {
  const router = useRouter();
  const [mode, setMode] = useState<Mode>("sign-in");
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
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
    const credentials = { email: email.trim(), password };
    const { data, error } =
      mode === "sign-in"
        ? await supabase.auth.signInWithPassword(credentials)
        : await supabase.auth.signUp(credentials);
    setLoading(false);
    if (error) {
      setMessage(error.message);
      return;
    }
    if (!data.session) {
      setMessage("Account created. Check your email to confirm it, then sign in.");
      return;
    }
    router.replace("/dashboard");
    router.refresh();
  };

  const switchMode = () => {
    setMode(mode === "sign-in" ? "sign-up" : "sign-in");
    setMessage(null);
  };

  return (
    <form onSubmit={(e) => void submit(e)} className="space-y-3 rounded-xl border border-bfm-border bg-bfm-card p-4">
      <label className="block text-sm">
        Email
        <input
          type="email"
          required
          autoComplete="email"
          value={email}
          onChange={(e) => setEmail(e.target.value)}
          className="mt-1 w-full rounded-lg border border-bfm-border bg-bfm-bg-2 px-3 py-2 text-white"
        />
      </label>
      <label className="block text-sm">
        Password
        <input
          type="password"
          required
          minLength={MIN_PASSWORD_LENGTH}
          autoComplete={mode === "sign-in" ? "current-password" : "new-password"}
          value={password}
          onChange={(e) => setPassword(e.target.value)}
          className="mt-1 w-full rounded-lg border border-bfm-border bg-bfm-bg-2 px-3 py-2 text-white"
        />
      </label>
      <button
        type="submit"
        disabled={loading}
        className="w-full rounded-lg bg-bfm-accent px-4 py-2 text-sm font-medium text-white disabled:opacity-50"
      >
        {loading ? "Please wait…" : mode === "sign-in" ? "Sign in" : "Create account"}
      </button>
      <button type="button" onClick={switchMode} className="w-full text-sm text-bfm-accent hover:underline">
        {mode === "sign-in" ? "New here? Create an account" : "Already have an account? Sign in"}
      </button>
      {message && <p className="text-sm text-bfm-muted">{message}</p>}
    </form>
  );
}
