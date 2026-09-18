import Link from "next/link";

import { AuthNav } from "@/components/AuthNav";
import { LaunchOptInForm } from "@/components/LaunchOptInForm";

export const metadata = {
  title: "Free 1994 Memphis Vocal Stems — DISKLORDZ",
  description:
    "Download 25 cleaned, pitch-tuned phonk vocal chops. No mud, no phase cancellation.",
};

export default function LaunchVocalsPage() {
  return (
    <div className="min-h-screen bg-zinc-950 px-4 py-12 text-zinc-100">
      <AuthNav />
      <div className="mx-auto max-w-2xl text-center">
        <p className="text-xs uppercase tracking-widest text-amber-500/90">Freebie tier 0</p>
        <h1 className="mt-3 text-3xl font-bold">25 cleaned, pitch-tuned 1994 Memphis phonk vocal chops</h1>
        <p className="mt-4 text-zinc-400">
          No background noise. No phase cancellation. Key and BPM labeled — drop them straight into your
          DAW.
        </p>
      </div>
      <div className="mt-10">
        <LaunchOptInForm />
      </div>
      <p className="mx-auto mt-8 max-w-md text-center text-xs text-zinc-600">
        Prefer to hear the effect first?{" "}
        <Link href="/launch" className="text-zinc-400 underline hover:text-zinc-200">
          Open the soundboard
        </Link>
      </p>
    </div>
  );
}
