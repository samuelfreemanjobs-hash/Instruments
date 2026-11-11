"use client";

import Link from "next/link";
import { usePathname } from "next/navigation";

const PRIMARY = [
  { href: "/dashboard", label: "Dashboard" },
  { href: "/discover", label: "Discover" },
  { href: "/search", label: "Search" },
  { href: "/compare", label: "Compare" },
  { href: "/assistant", label: "Assistant" },
  { href: "/alerts", label: "Alerts" },
];

const MORE = [
  { href: "/categories", label: "Categories" },
  { href: "/intel", label: "Intel" },
  { href: "/autopilot", label: "Autopilot" },
  { href: "/coupons", label: "Coupons" },
  { href: "/budget", label: "Budget" },
  { href: "/calendar", label: "Calendar" },
  { href: "/wishlist", label: "Wishlist" },
  { href: "/settings", label: "Settings" },
  { href: "/admin", label: "Admin" },
];

export function ShellNav() {
  const pathname = usePathname();

  return (
    <nav className="sticky top-0 z-30 border-b border-bfm-border bg-bfm-bg/95 backdrop-blur-md">
      <div className="mx-auto flex max-w-5xl flex-col gap-2 px-4 py-2">
        <div className="flex gap-2 overflow-x-auto pb-1 [-ms-overflow-style:none] [scrollbar-width:none] [&::-webkit-scrollbar]:hidden">
          {PRIMARY.map(({ href, label }) => {
            const active = pathname === href || pathname.startsWith(`${href}/`);
            return (
              <Link
                key={href}
                href={href}
                className={`shrink-0 rounded-full border px-3 py-2 text-sm ${
                  active
                    ? "border-bfm-accent bg-bfm-accent/15 text-white"
                    : "border-bfm-border text-bfm-muted hover:border-bfm-accent/50"
                }`}
              >
                {label}
              </Link>
            );
          })}
        </div>
        <div className="flex flex-wrap gap-2 text-xs">
          {MORE.map(({ href, label }) => (
            <Link key={href} href={href} className="text-bfm-muted hover:text-bfm-accent-2">
              {label}
            </Link>
          ))}
        </div>
      </div>
    </nav>
  );
}
