"use client";

import { useState } from "react";

export function LaunchOptInForm() {
  const [email, setEmail] = useState("");
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const submit = async (e: React.FormEvent) => {
    e.preventDefault();
    setLoading(true);
    setError(null);
    try {
      const res = await fetch("/api/launch/opt-in", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ email }),
      });
      const data = (await res.json()) as { ok?: boolean; error?: string; redirect?: string };
      if (!res.ok || !data.ok) {
        setError(data.error ?? "Could not submit — try again.");
        return;
      }
      window.location.href = data.redirect ?? "/launch/thank-you";
    } catch {
      setError("Network error — try again.");
    } finally {
      setLoading(false);
    }
  };

  return (
    <form onSubmit={(e) => void submit(e)} className="mx-auto max-w-md space-y-4">
      <label className="block text-sm text-zinc-300">
        Email
        <input
          type="email"
          required
          maxLength={320}
          value={email}
          onChange={(e) => setEmail(e.target.value)}
          placeholder="you@producer.email"
          className="mt-2 w-full rounded-lg border border-zinc-700 bg-zinc-950 px-4 py-3 text-zinc-100 outline-none focus:border-amber-500"
        />
      </label>
      {error && <p className="text-sm text-red-400">{error}</p>}
      <button
        type="submit"
        disabled={loading}
        className="w-full rounded-lg bg-amber-500 py-3 text-sm font-semibold text-zinc-950 hover:bg-amber-400 disabled:opacity-60"
      >
        {loading ? "Sending…" : "Send me the stems"}
      </button>
    </form>
  );
}
