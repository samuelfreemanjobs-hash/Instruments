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

  if (!ready || !email) {
    return (
      <Link href="/login" className="font-condensed text-sm tracking-wide text-ivory uppercase hover:text-gold">
        Account
      </Link>
    );
  }

  return (
    <div className="flex items-center gap-3">
      <Link href="/account" className="font-condensed text-sm tracking-wide text-ivory uppercase hover:text-gold">
        Account
      </Link>
      <button
        type="button"
        onClick={() => void signOut()}
        className="min-h-11 font-condensed text-sm tracking-wide text-gold uppercase"
      >
        Sign out
      </button>
    </div>
  );
}
