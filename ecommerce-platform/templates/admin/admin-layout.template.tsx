"use client";

import Link from "next/link";
import { usePathname } from "next/navigation";
import { adminNav } from "@/lib/mock/admin-mock-data";
import { cn } from "@/lib/utils";
import { LayoutDashboard, Store } from "lucide-react";

export function AdminShell({ children }: { children: React.ReactNode }) {
  const pathname = usePathname();

  return (
    <div className="flex min-h-screen bg-slate-100">
      <aside className="flex w-64 flex-col border-r border-slate-200 bg-slate-900 text-slate-100">
        <div className="flex items-center gap-2 border-b border-slate-700 px-4 py-5 font-semibold">
          <LayoutDashboard className="h-5 w-5" />
          Shop Admin
        </div>
        <nav className="flex-1 space-y-0.5 overflow-y-auto p-3 text-sm">
          {adminNav.map((item) => {
            const active =
              pathname === item.href ||
              (item.href !== "/admin" && pathname.startsWith(item.href));
            return (
              <Link
                key={item.href}
                href={item.href}
                className={cn(
                  "block rounded-md px-3 py-2 transition-colors",
                  active ? "bg-slate-700 text-white" : "text-slate-300 hover:bg-slate-800",
                )}
              >
                {item.label}
              </Link>
            );
          })}
        </nav>
        <Link
          href="/"
          className="flex items-center gap-2 border-t border-slate-700 px-4 py-3 text-sm text-slate-400 hover:text-white"
        >
          <Store className="h-4 w-4" />
          View storefront
        </Link>
      </aside>
      <main className="flex-1 overflow-auto p-6 md:p-8">{children}</main>
    </div>
  );
}
