"use client";

import Link from "next/link";
import { useRouter } from "next/navigation";
import { useState, useRef, useEffect } from "react";
import { User } from "lucide-react";
import { logoutAction } from "@/lib/actions/auth";
import type { SessionPayload } from "@/lib/auth/session";

export function AccountNav({ session }: { session: SessionPayload | null }) {
  const [open, setOpen] = useState(false);
  const ref = useRef<HTMLDivElement>(null);
  const router = useRouter();

  useEffect(() => {
    function onDoc(e: MouseEvent) {
      if (ref.current && !ref.current.contains(e.target as Node)) setOpen(false);
    }
    document.addEventListener("mousedown", onDoc);
    return () => document.removeEventListener("mousedown", onDoc);
  }, []);

  async function logout() {
    await logoutAction();
    router.refresh();
  }

  if (!session) {
    return (
      <Link href="/auth/login" className="flex items-center gap-1 hover:text-slate-600">
        <User className="h-4 w-4" />
        Account
      </Link>
    );
  }

  return (
    <div className="relative" ref={ref}>
      <button
        type="button"
        onClick={() => setOpen((v) => !v)}
        className="flex items-center gap-1 hover:text-slate-600"
      >
        <User className="h-4 w-4" />
        {session.name ?? session.email.split("@")[0]}
      </button>
      {open ? (
        <div className="absolute right-0 z-50 mt-2 w-48 rounded-md border border-slate-200 bg-white py-1 text-sm shadow-lg">
          <p className="border-b border-slate-100 px-3 py-2 text-xs text-slate-500">{session.email}</p>
          <Link
            href="/account/profile"
            className="block px-3 py-2 hover:bg-slate-50"
            onClick={() => setOpen(false)}
          >
            Profile
          </Link>
          {session.role === "ADMIN" ? (
            <Link
              href="/admin"
              className="block px-3 py-2 hover:bg-slate-50"
              onClick={() => setOpen(false)}
            >
              Admin dashboard
            </Link>
          ) : null}
          <button
            type="button"
            onClick={() => void logout()}
            className="block w-full px-3 py-2 text-left hover:bg-slate-50"
          >
            Log out
          </button>
        </div>
      ) : null}
    </div>
  );
}
