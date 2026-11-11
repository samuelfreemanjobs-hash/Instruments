"use client";

import Link from "next/link";
import { usePathname } from "next/navigation";
import { useState } from "react";

const PRIMARY = [
  { href: "/dashboard", label: "Home" },
  { href: "/search", label: "Search" },
  { href: "/compare", label: "Compare" },
  { href: "/wishlist", label: "Wishlist" },
  { href: "/alerts", label: "Alerts" },
];

const MORE = [
  { href: "/discover", label: "Discover" },
  { href: "/assistant", label: "Assistant" },
  { href: "/categories", label: "Categories" },
  { href: "/coupons", label: "Coupons" },
  { href: "/budget", label: "Budget" },
  { href: "/calendar", label: "Calendar" },
  { href: "/intel", label: "Intel" },
  { href: "/autopilot", label: "Autopilot" },
  { href: "/settings", label: "Settings" },
  { href: "/admin", label: "Admin" },
];

function isActive(pathname: string, href: string) {
  return pathname === href || pathname.startsWith(`${href}/`);
}

function pillClass(active: boolean) {
  return `rounded-full border px-4 py-2.5 text-base font-medium ${
    active
      ? "border-bfm-accent bg-bfm-accent text-white"
      : "border-bfm-border text-white hover:border-bfm-accent"
  }`;
}

export function ShellNav() {
  const pathname = usePathname();
  const [moreOpen, setMoreOpen] = useState(false);
  const onMorePage = MORE.some(({ href }) => isActive(pathname, href));

  return (
    <nav className="sticky top-0 z-30 border-b border-bfm-border bg-bfm-bg/95 backdrop-blur-md">
      <div className="mx-auto flex max-w-5xl flex-col gap-3 px-4 py-3">
        <div className="flex flex-wrap gap-2">
          {PRIMARY.map(({ href, label }) => (
            <Link
              key={href}
              href={href}
              onClick={() => setMoreOpen(false)}
              className={pillClass(isActive(pathname, href))}
            >
              {label}
            </Link>
          ))}
          <button
            type="button"
            aria-expanded={moreOpen}
            onClick={() => setMoreOpen(!moreOpen)}
            className={pillClass(onMorePage && !moreOpen)}
          >
            {moreOpen ? "Close ✕" : "More ▾"}
          </button>
        </div>
        {moreOpen && (
          <div className="flex flex-wrap gap-2 border-t border-bfm-border pt-3">
            {MORE.map(({ href, label }) => (
              <Link
                key={href}
                href={href}
                onClick={() => setMoreOpen(false)}
                className={pillClass(isActive(pathname, href))}
              >
                {label}
              </Link>
            ))}
          </div>
        )}
      </div>
    </nav>
  );
}
