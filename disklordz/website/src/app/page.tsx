import Image from "next/image";
import Link from "next/link";

import { readMemphisVault } from "@/lib/catalog/memphis-vault";

export default function Home() {
  const vault = readMemphisVault();

  return (
    <div>
      <section className="border-b border-gold/20">
        <div className="mx-auto grid max-w-6xl items-center gap-8 px-4 py-14 md:grid-cols-[1.1fr_0.9fr] md:py-20">
          <div>
            <p className="font-condensed text-sm tracking-[0.28em] text-gold uppercase">DiskLordz Audio</p>
            <h1 className="mt-3 font-display text-5xl leading-none text-ivory md:text-7xl">
              Rule the underground.
            </h1>
            <p className="mt-4 max-w-xl text-lg text-ivory/85">
              Heavy drums. Legendary sounds. Serious production tools.
            </p>
            <p className="mt-3 max-w-xl text-ivory/70">
              Explore underground drum kits and the Drum Factory, the live generator for original one-shots.
            </p>
            <div className="mt-8 flex flex-wrap gap-3">
              <Link
                href="/sounds/memphis-vault-vol-1"
                className="inline-flex min-h-11 items-center bg-gold px-4 font-condensed font-semibold tracking-wide text-obsidian uppercase"
              >
                Explore sounds
              </Link>
              <Link
                href="/drum-factory"
                className="inline-flex min-h-11 items-center border border-gold px-4 font-condensed font-semibold tracking-wide text-gold uppercase"
              >
                Launch Drum Factory
              </Link>
            </div>
          </div>
          <Image
            src="/brand/disklordz-crest-badge.png"
            alt="DiskLordz crest with a crowned floppy disk, drum machine, and synthesizer."
            width={512}
            height={279}
            priority
            className="h-auto w-full"
          />
        </div>
      </section>

      <section className="mx-auto max-w-6xl px-4 py-14">
        <p className="font-condensed text-sm tracking-[0.22em] text-gold uppercase">Catalog</p>
        <h2 className="mt-2 font-display text-4xl text-ivory">Memphis Vault Vol. 1</h2>
        <article className="mt-6 grid gap-6 border border-gold/30 bg-charcoal p-4 md:grid-cols-[220px_1fr] md:p-6">
          {/* eslint-disable-next-line @next/next/no-img-element */}
          <img
            src="/api/catalog/memphis-vault-vol-1?file=cover.jpg"
            alt="Cassette cover for Memphis Vault Vol. 1"
            className="w-full"
          />
          <div>
            <p className="text-ivory/80">{vault.tagline}</p>
            <p className="mt-3 font-mono text-sm text-ivory/70">
              {vault.count} one-shots · {vault.sampleRate / 1000} kHz · {vault.bitDepth}-bit · mono WAV
            </p>
            <p className="mt-3 text-sm text-ivory/70">
              Original synthesis. Draft catalog pack. Price is not set, and checkout is not open.
            </p>
            <Link
              href="/sounds/memphis-vault-vol-1"
              className="mt-6 inline-flex min-h-11 items-center bg-gold px-4 font-condensed font-semibold tracking-wide text-obsidian uppercase"
            >
              View the pack
            </Link>
          </div>
        </article>
      </section>

      <section className="bg-forest">
        <div className="mx-auto max-w-6xl px-4 py-14">
          <h2 className="font-display text-4xl text-ivory">Don&apos;t just buy sounds. Create your own.</h2>
          <p className="mt-3 max-w-2xl text-ivory/85">
            Drum Factory renders original kicks, snares, hats, and claps from a prompt, then exports a WAV zip.
            The knobs on design mockups are not this control.
          </p>
          <Link
            href="/drum-factory"
            className="mt-6 inline-flex min-h-11 items-center bg-gold px-4 font-condensed font-semibold tracking-wide text-obsidian uppercase"
          >
            Open the live generator
          </Link>
        </div>
      </section>

      <section className="mx-auto max-w-6xl px-4 py-14">
        <h2 className="font-display text-4xl text-ivory">The underground producer&apos;s toolbox.</h2>
        <ul className="mt-6 grid gap-4 md:grid-cols-3">
          <li className="border border-gold/30 bg-charcoal p-4">
            <p className="font-condensed text-xs tracking-[0.16em] text-gold uppercase">In development</p>
            <h3 className="mt-2 font-display text-2xl text-ivory">Chord Engine</h3>
            <p className="mt-2 text-sm text-ivory/75">
              Windows VST3 is the build target. No public installer, trial, or purchase yet.
            </p>
          </li>
          <li className="border border-gold/20 bg-charcoal p-4">
            <p className="font-condensed text-xs tracking-[0.16em] text-ivory/50 uppercase">Coming soon</p>
            <h3 className="mt-2 font-display text-2xl text-ivory">Trap Lord Synth</h3>
            <p className="mt-2 text-sm text-ivory/75">Not available to buy or download.</p>
          </li>
          <li className="border border-gold/30 bg-charcoal p-4">
            <p className="font-condensed text-xs tracking-[0.16em] text-gold uppercase">Built</p>
            <h3 className="mt-2 font-display text-2xl text-ivory">
              <Link href="/plugins/memphis-phonk-factory" className="hover:text-gold">
                Memphis Phonk Factory
              </Link>
            </h3>
            <p className="mt-2 text-sm text-ivory/75">
              The Dust VST. Original phonk drums and an MPC pack export. No public installer yet.
            </p>
          </li>
        </ul>
      </section>
    </div>
  );
}
