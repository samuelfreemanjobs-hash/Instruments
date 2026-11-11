"use client";

import { useState } from "react";

import { sendMagicLink } from "./actions";

export function LoginForm() {
  const [email, setEmail] = useState("");
  const [message, setMessage] = useState<string | null>(null);
  const [hint, setHint] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  const submit = async (e: React.FormEvent) => {
    e.preventDefault();
    setLoading(true);
    setMessage(null);
    setHint(null);

    const res = await sendMagicLink({
      email,
      origin: window.location.origin,
      next: "/dashboard",
    });

    setLoading(false);
    if (!res.ok) {
      if (res.error === "redirect_not_allowed") {
        setMessage(
          `Supabase rejected the redirect URL. ${"detail" in res ? res.detail : ""} Whitelist: ${window.location.origin}/auth/callback`,
        );
        setHint(res.hint ?? null);
      } else if (res.error === "rate_limited") {
        setMessage("Email rate limit exceeded on Supabase’s built-in mailer.");
        setHint(res.hint ?? "Wait about an hour, then try again — or add custom SMTP in Supabase.");
      } else if (res.error === "invalid_email") {
        setMessage("Enter a valid email address.");
      } else {
        setMessage("detail" in res && res.detail ? res.detail : "Could not send magic link.");
      }
      return;
    }

    setMessage("If an account exists (or is new), Supabase is sending a sign-in link.");
    setHint(res.hint ?? null);
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
      <button
        type="submit"
        disabled={loading}
        className="w-full rounded-lg bg-bfm-accent px-4 py-2 text-sm font-medium text-white disabled:opacity-50"
      >
        {loading ? "Sending…" : "Email magic link"}
      </button>
      {message && <p className="text-sm text-bfm-muted">{message}</p>}
      {hint && <p className="text-xs text-bfm-muted">{hint}</p>}
    </form>
  );
}
