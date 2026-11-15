import type { Metadata } from "next";
import Link from "next/link";

export const metadata: Metadata = {
  title: "Plugins",
  description:
    "Memphis Phonk Factory is built. Chord Engine is in development. No installers are for sale.",
};

const PLUGINS = [
  {
    name: "Memphis Phonk Factory",
    href: "/plugins/memphis-phonk-factory",
    status: "Built",
    detail:
      "The Dust VST. Original phonk one-shots, loops, and an MPC pack export. No public installer yet.",
  },
  {
    name: "Chord Engine",
    href: null,
    status: "In development",
    detail: "Windows VST3 is the build target. No public installer, trial, or purchase yet.",
  },
  {
    name: "Trap Lord Synth",
    href: null,
    status: "Coming soon",
    detail: "Not available to buy or download.",
  },
];

export default function PluginsPage() {
  return (
    <div className="mx-auto max-w-6xl px-4 py-12">
      <h1 className="font-display text-5xl text-ivory">Plugins</h1>
      <ul className="mt-8 grid gap-4 md:grid-cols-3">
        {PLUGINS.map((plugin) => (
          <li key={plugin.name} className="border border-gold/30 bg-charcoal p-4">
            <p className="font-condensed text-xs tracking-[0.16em] text-gold uppercase">{plugin.status}</p>
            <h2 className="mt-2 font-display text-2xl text-ivory">
              {plugin.href ? (
                <Link href={plugin.href} className="hover:text-gold">
                  {plugin.name}
                </Link>
              ) : (
                plugin.name
              )}
            </h2>
            <p className="mt-2 text-sm text-ivory/75">{plugin.detail}</p>
          </li>
        ))}
      </ul>
    </div>
  );
}
