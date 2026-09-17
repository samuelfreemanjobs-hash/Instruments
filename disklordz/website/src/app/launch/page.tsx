import Link from "next/link";

import { AuthNav } from "@/components/AuthNav";
import { CassetteSmashSoundboard } from "@/components/CassetteSmashSoundboard";

export const metadata = {
  title: "DISKLORDZ Launch — Memphis Tape",
  description:
    "Preview Cassette Smash on phonk stems, then grab free 1994 vocal chops.",
};

export default function LaunchPage() {
  return (
    <div className="min-h-screen bg-zinc-950 px-4 py-12 text-zinc-100">
      <AuthNav />
      <header className="mx-auto max-w-3xl text-center">
        <p className="text-xs uppercase tracking-[0.2em] text-amber-500/90">Master launch canvas</p>
        <h1 className="mt-3 text-3xl font-bold tracking-tight sm:text-4xl">
          Dirty the signal path before you drop the stems
        </h1>
        <p className="mt-4 text-zinc-400">
          Toggle Cassette Smash, crank Drive, then claim 25 cleaned Memphis vocal chops — key and BPM
          labeled for your DAW.
        </p>
        <Link
          href="/launch/vocals"
          className="mt-6 inline-block rounded-lg bg-amber-500 px-6 py-3 text-sm font-semibold text-zinc-950 hover:bg-amber-400"
        >
          Get the free vocal vault
        </Link>
      </header>

      <div className="mt-12">
        <CassetteSmashSoundboard />
      </div>

      <p className="mx-auto mt-10 max-w-xl text-center text-xs text-zinc-600">
        Drum kit SaaS still lives on{" "}
        <Link href="/" className="text-zinc-400 underline hover:text-zinc-200">
          the home page
        </Link>
        . Plugin binaries and IR packs ship on storefront SKUs — see docs/DISKLORDZ_MASTER_LAUNCH_CANVAS.md.
      </p>
    </div>
  );
}
