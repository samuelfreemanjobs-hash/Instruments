import type { Metadata } from "next";
import Link from "next/link";

export const metadata: Metadata = {
  title: "Learn",
  description: "What DiskLordz can generate today, and which lessons are not written yet.",
};

export default function LearnPage() {
  return (
    <div className="mx-auto max-w-3xl px-4 py-12">
      <h1 className="font-display text-5xl text-ivory">Learn</h1>
      <p className="mt-4 text-ivory/80">
        The live path is the Drum Factory: type a prompt, audition the one-shots, and download a WAV zip. The{" "}
        <Link href="/daw-inbox" className="text-gold underline">
          DAW inbox
        </Link>{" "}
        explains how those files get into a session.
      </p>
      <p className="mt-3 text-ivory/80">
        Lessons on Memphis drums, phonk cowbells, and 808 design are not written yet. Those engines are not
        separate labs.
      </p>
    </div>
  );
}
