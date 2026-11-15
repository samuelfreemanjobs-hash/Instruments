import type { Metadata } from "next";
import Link from "next/link";

export const metadata: Metadata = {
  title: "Free sounds",
  description: "Generate original one-shots in Drum Factory within the server daily limit.",
};

export default function FreePage() {
  return (
    <div className="mx-auto max-w-3xl px-4 py-12">
      <h1 className="font-display text-5xl text-ivory">Free sounds. Real heat.</h1>
      <p className="mt-4 text-ivory/80">
        Drum Factory generates original WAV one-shots. Guests are limited per IP. The default daily cap is the
        server value of SAAS_DAILY_GEN_LIMIT, which starts at 20. Signed-in accounts use the credit ledger when
        Supabase is configured.
      </p>
      <p className="mt-3 text-ivory/80">
        Memphis Vault Vol. 1 is a draft catalog pack. It is not a free download while checkout is closed.
      </p>
      <Link
        href="/drum-factory"
        className="mt-8 inline-flex min-h-11 items-center bg-gold px-4 font-condensed font-semibold tracking-wide text-obsidian uppercase"
      >
        Open Drum Factory
      </Link>
    </div>
  );
}
