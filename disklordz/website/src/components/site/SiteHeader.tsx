"use client";

import Image from "next/image";
import Link from "next/link";
import { useState } from "react";

import { AuthNav } from "@/components/AuthNav";

const LINKS = [
  { href: "/sounds", label: "Sounds" },
  { href: "/plugins", label: "Plugins" },
  { href: "/drum-factory", label: "Drum Factory" },
  { href: "/free", label: "Free Sounds" },
  { href: "/learn", label: "Learn" },
];

export function SiteHeader() {
  const [open, setOpen] = useState(false);

  return (
    <header className="sticky top-0 z-40 border-b border-gold/30 bg-obsidian/95">
      <div className="mx-auto flex max-w-6xl items-center gap-4 px-4 py-3">
        <Link href="/" className="flex items-center gap-2">
          <Image
            src="/brand/disklordz-floppy-crown-maroon.png"
            alt=""
            width={88}
            height={48}
            className="h-12 w-auto bg-ivory object-contain p-1"
          />
          <span className="font-display text-lg tracking-wide text-ivory">DiskLordz</span>
        </Link>
        <nav className="ml-4 hidden items-center gap-5 lg:flex" aria-label="Primary">
          {LINKS.map((link) => (
            <Link
              key={link.href}
              href={link.href}
              className="font-condensed text-sm font-semibold tracking-[0.16em] text-ivory/80 uppercase hover:text-gold"
            >
              {link.label}
            </Link>
          ))}
        </nav>
        <div className="ml-auto flex items-center gap-3">
          <AuthNav />
          <Link
            href="/drum-factory"
            className="hidden min-h-11 items-center bg-gold px-3 font-condensed text-sm font-semibold tracking-wide text-obsidian uppercase sm:inline-flex"
          >
            Launch Drum Factory
          </Link>
          <button
            type="button"
            className="min-h-11 min-w-11 font-condensed text-sm tracking-wide text-gold uppercase lg:hidden"
            aria-expanded={open}
            aria-controls="mobile-nav"
            onClick={() => setOpen((value) => !value)}
          >
            {open ? "Close" : "Menu"}
          </button>
        </div>
      </div>
      {open ? (
        <nav id="mobile-nav" className="border-t border-gold/20 px-4 py-3 lg:hidden" aria-label="Mobile">
          <ul className="flex flex-col">
            {LINKS.map((link) => (
              <li key={link.href}>
                <Link
                  href={link.href}
                  className="block min-h-11 py-2 font-condensed tracking-[0.14em] text-ivory uppercase"
                  onClick={() => setOpen(false)}
                >
                  {link.label}
                </Link>
              </li>
            ))}
          </ul>
        </nav>
      ) : null}
    </header>
  );
}
