import Image from "next/image";
import Link from "next/link";

const storeUrl = process.env.LAUNCH_MEMPHIS_VAULT_URL;

export function MemphisVaultKitPromo() {
  return (
    <section className="mx-auto mt-14 max-w-3xl rounded-2xl border border-zinc-800 bg-zinc-900/40 p-6 sm:flex sm:gap-6">
      <div className="relative mx-auto h-48 w-48 shrink-0 sm:mx-0">
        <Image
          src="/launch/memphis-vault-vol1-cover.png"
          alt="Memphis Vault Vol. 1 — underground phonk sample pack cover"
          fill
          className="rounded-lg object-cover"
          sizes="192px"
        />
      </div>
      <div className="mt-6 text-center sm:mt-0 sm:text-left">
        <p className="text-xs uppercase tracking-widest text-zinc-500">Legacy catalog · back in the launch stack</p>
        <h2 className="mt-2 text-xl font-semibold text-zinc-50">Memphis Vault Vol. 1</h2>
        <p className="mt-2 text-sm text-zinc-400">
          Underground phonk one-shots and drums from the earlier Disklordz era. Internal working title in
          brand notes was <span className="text-zinc-300">Graveyard Shift</span> — same OG Memphis lane as
          this launch (skulls, cassette, 12-bit knock).
        </p>
        <p className="mt-2 text-xs text-zinc-600">Produced by AJ OG · folders: kicks, 808s, snares, perc, FX</p>
        {storeUrl ? (
          <a
            href={storeUrl}
            className="mt-4 inline-block rounded-lg border border-amber-500/50 px-5 py-2.5 text-sm font-medium text-amber-400 hover:bg-amber-500/10"
          >
            Get Memphis Vault Vol. 1
          </a>
        ) : (
          <p className="mt-4 text-xs text-zinc-600">
            Set <code className="text-zinc-500">LAUNCH_MEMPHIS_VAULT_URL</code> for the storefront link, or
            bundle with the <Link href="/launch/thank-you" className="text-zinc-400 underline">Vault tier</Link>.
          </p>
        )}
      </div>
    </section>
  );
}
