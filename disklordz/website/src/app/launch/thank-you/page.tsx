import Link from "next/link";

import { AuthNav } from "@/components/AuthNav";

const tripwireUrl =
  process.env.LAUNCH_TRIPWIRE_CASSETTE_URL ?? "https://example.com/cassette-smash-lite";
const downsellUrl =
  process.env.LAUNCH_TRIPWIRE_IR_URL ?? "https://example.com/tascam-ir-pack";
const flagshipUrl =
  process.env.LAUNCH_FLAGSHIP_URL ?? "https://example.com/memphis-tape-fx";

export const metadata = {
  title: "Your stems are on the way — DISKLORDZ",
  description: "Thank-you page tripwire: Cassette Smash Lite and Memphis Tape FX.",
};

export default function LaunchThankYouPage() {
  return (
    <div className="min-h-screen bg-zinc-950 px-4 py-12 text-zinc-100">
      <AuthNav />
      <article className="mx-auto max-w-2xl rounded-2xl border border-zinc-800 bg-zinc-900/50 p-8">
        <p className="text-xs uppercase tracking-widest text-emerald-500/90">Check your inbox</p>
        <h1 className="mt-3 text-2xl font-bold">Your stems are flying to your inbox</h1>
        <p className="mt-4 text-zinc-400">
          But your mix will still sound like a pristine digital synth unless you dirty up the signal path.
          Grab the two-knob <strong className="text-zinc-200">Cassette Smash</strong> tool before you close
          this tab.
        </p>
        <div className="mt-8 flex flex-col gap-3 sm:flex-row">
          <a
            href={tripwireUrl}
            className="rounded-lg bg-amber-500 px-5 py-3 text-center text-sm font-semibold text-zinc-950 hover:bg-amber-400"
          >
            Cassette Smash Lite — $17
          </a>
          <a
            href={flagshipUrl}
            className="rounded-lg border border-zinc-600 px-5 py-3 text-center text-sm font-medium text-zinc-200 hover:border-zinc-400"
          >
            Memphis Tape FX (early bird)
          </a>
        </div>
        <p className="mt-6 text-sm text-zinc-500">
          Not ready for the plugin?{" "}
          <a href={downsellUrl} className="text-amber-500/90 underline hover:text-amber-400">
            Tascam 4-track IR pack — $9
          </a>
        </p>
        <Link href="/launch" className="mt-8 inline-block text-sm text-zinc-400 underline hover:text-zinc-200">
          Back to soundboard
        </Link>
      </article>
    </div>
  );
}
