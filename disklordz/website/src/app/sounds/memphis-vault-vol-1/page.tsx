import type { Metadata } from "next";
import Link from "next/link";

import { PreviewList } from "@/components/catalog/PreviewList";
import { readMemphisVault } from "@/lib/catalog/memphis-vault";

export const metadata: Metadata = {
  title: "Memphis Vault Vol. 1",
  description: "Original synthesized phonk one-shots. 44.1 kHz, 16-bit, mono WAV. Draft catalog pack.",
};

export default function MemphisVaultPage() {
  const vault = readMemphisVault();

  return (
    <article className="mx-auto max-w-6xl px-4 py-12">
      <p className="font-condensed text-sm tracking-[0.2em] text-gold uppercase">
        <Link href="/sounds" className="hover:text-ivory">
          Sounds
        </Link>
      </p>
      <div className="mt-4 grid gap-8 md:grid-cols-[280px_1fr]">
        {/* eslint-disable-next-line @next/next/no-img-element */}
        <img
          src="/api/catalog/memphis-vault-vol-1?file=cover.jpg"
          alt="Cassette cover for Memphis Vault Vol. 1"
          className="w-full border border-gold/30"
        />
        <div>
          <h1 className="font-display text-4xl text-ivory md:text-5xl">{vault.product}</h1>
          <p className="mt-3 text-lg text-ivory/85">{vault.tagline}</p>
          <p className="mt-3 max-w-xl text-ivory/75">
            Original synthesized phonk one-shots. Kicks, 808s, snares, claps, hats, cowbells, percussion, and FX.
            Not sampled from records or drum machines.
          </p>
          <p className="mt-4 font-mono text-sm text-ivory/80">
            {vault.count} sounds · {vault.sampleRate / 1000} kHz · {vault.bitDepth}-bit ·{" "}
            {vault.channels === 1 ? "mono" : `${vault.channels} ch`} WAV
          </p>
          <p className="mt-4 border border-gold/40 bg-charcoal px-3 py-2 font-condensed text-sm tracking-wide text-gold uppercase">
            Draft · price not set · checkout closed
          </p>
          <p className="mt-4 text-sm text-ivory/70">{vault.license}</p>
        </div>
      </div>

      <section className="mt-10">
        <h2 className="font-display text-2xl text-ivory">Audition</h2>
        <p className="mt-2 text-sm text-ivory/70">Four real files from the pack. Playback starts when you press Play.</p>
        <div className="mt-4">
          <PreviewList tracks={vault.previews} />
        </div>
      </section>

      <section className="mt-10">
        <h2 className="font-display text-2xl text-ivory">Contents</h2>
        <ul className="mt-4 grid gap-3 sm:grid-cols-2 lg:grid-cols-4">
          {vault.folders.map((folder) => (
            <li key={folder.name} className="border border-gold/25 bg-charcoal px-3 py-3">
              <p className="font-condensed tracking-wide text-ivory uppercase">{folder.name.replace(/^\d+_/, "")}</p>
              <p className="font-mono text-sm text-gold">{folder.count}</p>
            </li>
          ))}
        </ul>
      </section>

      <p className="mt-10 text-sm text-ivory/75">
        Shape your own hits in the{" "}
        <Link href="/drum-factory" className="text-gold underline">
          Drum Factory
        </Link>
        .
      </p>
    </article>
  );
}
