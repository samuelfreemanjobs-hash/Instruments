"use client";

import Link from "next/link";
import { useEffect, useState } from "react";

import { createClient } from "@/lib/supabase/client";

export function AuthNav() {
  const [email, setEmail] = useState<string | null>(null);
  const [ready, setReady] = useState(false);

  useEffect(() => {
    const supabase = createClient();
    if (!supabase) {
      setReady(true);
      return;
    }
    supabase.auth.getUser().then(({ data }) => {
      setEmail(data.user?.email ?? null);
      setReady(true);
    });
    const { data: sub } = supabase.auth.onAuthStateChange((_event, session) => {
      setEmail(session?.user?.email ?? null);
    });
    return () => sub.subscription.unsubscribe();
  }, []);

  async function signOut() {
    const supabase = createClient();
    if (supabase) {
      await supabase.auth.signOut();
      setEmail(null);
    }
  }

  if (!ready) return null;

  return (
    <nav className="mx-auto flex max-w-3xl items-center justify-end gap-4 pb-4 text-sm">
      {email ? (
        <>
          <Link href="/account" className="text-zinc-400 hover:text-zinc-200">
            Account
          </Link>
          <span className="text-zinc-500">{email}</span>
          <button
            type="button"
            onClick={signOut}
            className="text-emerald-400 hover:text-emerald-300"
          >
            Sign out
          </button>
        </>
      ) : (
        <Link href="/login" className="text-emerald-400 hover:text-emerald-300">
          Sign in
        </Link>
      )}
    </nav>
  );
}
