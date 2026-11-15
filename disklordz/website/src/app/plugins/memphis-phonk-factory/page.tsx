import type { Metadata } from "next";
import Link from "next/link";

export const metadata: Metadata = {
  title: "Memphis Phonk Factory",
  description:
    "Memphis Phonk Factory is the Dust VST: original phonk one-shots, loops, and an MPC pack export. No public installer yet.",
};

const EXPORTS = [
  "20 one-shots",
  "A loop at the session tempo, 1, 2, or 4 bars",
  "Optional classic loops at 112, 118, and 124 BPM",
  "Stems for kick, snare, hats, and mix",
  "Pattern MIDI and a pack manifest",
];

export default function MemphisPhonkFactoryPage() {
  return (
    <div className="mx-auto max-w-3xl px-4 py-12">
      <p className="font-condensed text-sm tracking-[0.22em] text-gold uppercase">VST3 and standalone</p>
      <h1 className="mt-2 font-display text-5xl text-ivory">Memphis Phonk Factory</h1>
      <p className="mt-4 text-lg text-ivory/85">
        The Dust VST. It writes original Memphis phonk drums, then exports an MPC-ready folder.
      </p>
      <p className="mt-3 text-ivory/75">
        Dust is one control. It sets tape drive, hiss, wow and flutter, and bit crush. Variation changes the
        seed. Tighten shortens kick and snare decay to the loop tempo.
      </p>

      <section className="mt-8 border border-gold/30 bg-charcoal p-5">
        <h2 className="font-display text-2xl text-ivory">What an export contains</h2>
        <ul className="mt-3 space-y-2 text-ivory/80">
          {EXPORTS.map((item) => (
            <li key={item}>{item}</li>
          ))}
        </ul>
        <p className="mt-4 text-sm text-ivory/70">
          The pattern grid is 16 steps for kick, snare, and open hat. Half-time preview and DAW tempo sync are
          in the plugin.
        </p>
      </section>

      <p className="mt-6 text-sm text-ivory/70">
        Built in this project. This page does not sell it and does not host an installer.
      </p>
      <Link
        href="/plugins"
        className="mt-6 inline-flex min-h-11 items-center font-condensed text-sm font-semibold tracking-wide text-gold uppercase"
      >
        All plugins
      </Link>
    </div>
  );
}
