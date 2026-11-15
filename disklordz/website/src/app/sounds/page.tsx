import type { Metadata } from "next";
import Link from "next/link";

import { readMemphisVault } from "@/lib/catalog/memphis-vault";

export const metadata: Metadata = {
  title: "Sounds",
  description: "DiskLordz catalog. Memphis Vault Vol. 1 is the first pack and is not for sale yet.",
};

export default function SoundsPage() {
  const vault = readMemphisVault();

  return (
    <div className="mx-auto max-w-6xl px-4 py-12">
      <h1 className="font-display text-5xl text-ivory">Sounds</h1>
      <p className="mt-3 max-w-xl text-ivory/75">
        One draft pack is in the catalog. More kits appear after they are rendered, checked, and auditioned.
      </p>
      <Link
        href="/sounds/memphis-vault-vol-1"
        className="mt-8 grid max-w-3xl gap-4 border border-gold/30 bg-charcoal p-4 sm:grid-cols-[160px_1fr]"
      >
        {/* eslint-disable-next-line @next/next/no-img-element */}
        <img src="/api/catalog/memphis-vault-vol-1?file=cover.jpg" alt="" className="w-full" />
        <span>
          <span className="block font-display text-3xl text-ivory">{vault.product}</span>
          <span className="mt-2 block text-ivory/75">{vault.tagline}</span>
          <span className="mt-2 block font-mono text-sm text-gold">
            {vault.count} sounds · draft · price not set
          </span>
        </span>
      </Link>
    </div>
  );
}
