"use client";

import Link from "next/link";
import { usePathname } from "next/navigation";
import { useEffect, useState } from "react";

import { createClient } from "@/lib/supabase/client";

const LINKS = [
  { href: "/", label: "Factory" },
  { href: "/studio", label: "Prompt studio" },
  { href: "/daw-inbox", label: "DAW inbox" },
] as const;

export function SiteChrome() {
  const pathname = usePathname();
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

  return (
    <header className="site-chrome">
      <Link href="/" className="site-brand">
        DiskLordz
      </Link>
      <nav className="site-nav" aria-label="Primary">
        {LINKS.map((link) => {
          const active =
            link.href === "/"
              ? pathname === "/"
              : pathname === link.href || pathname.startsWith(`${link.href}/`);
          return (
            <Link
              key={link.href}
              href={link.href}
              className={active ? "site-nav-link is-active" : "site-nav-link"}
            >
              {link.label}
            </Link>
          );
        })}
      </nav>
      <div className="site-auth">
        {!ready ? null : email ? (
          <>
            <Link href="/account" className="site-nav-link">
              Account
            </Link>
            <button type="button" className="site-nav-link" onClick={signOut}>
              Sign out
            </button>
          </>
        ) : (
          <Link href="/login" className="site-nav-cta">
            Sign in
          </Link>
        )}
      </div>
    </header>
  );
}
