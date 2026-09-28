import Link from "next/link";

export default function DawInboxPage() {
  return (
    <main className="mx-auto max-w-2xl px-4 py-12 text-zinc-100">
      <p className="text-sm font-medium uppercase tracking-widest text-emerald-400/90">
        WO-SAAS-016
      </p>
      <h1 className="mt-2 text-3xl font-semibold">DAW inbox</h1>
      <p className="mt-4 text-zinc-400">
        Two ways to get WAVs into your DAW without digging through ZIPs: pick a folder in the browser,
        or run the optional desktop watcher on your machine.
      </p>

      <section className="mt-8 space-y-3 rounded-xl border border-zinc-800 bg-zinc-900/50 p-5">
        <h2 className="text-lg font-semibold">In the browser</h2>
        <p className="text-sm text-zinc-400">
          After you generate a kit on the{" "}
          <Link href="/" className="text-emerald-400 hover:underline">home page</Link>, use{" "}
          <strong className="font-medium text-zinc-200">Save WAVs to folder</strong> (Chrome or Edge).
          Files land in a subfolder you choose — drag into your DAW.
        </p>
      </section>

      <section className="mt-6 space-y-3 rounded-xl border border-zinc-800 bg-zinc-900/50 p-5">
        <h2 className="text-lg font-semibold">Desktop watcher</h2>
        <p className="text-sm text-zinc-400">
          Watches <code className="text-zinc-300">~/Downloads</code> for{" "}
          <code className="text-zinc-300">disklordz-*.zip</code> and extracts to{" "}
          <code className="text-zinc-300">~/Music/Disklordz/Inbox/</code>.
        </p>
        <pre className="overflow-x-auto rounded-lg bg-zinc-950 p-4 text-xs text-zinc-300">
{`cd disklordz/daw-inbox
npm install
npm start`}
        </pre>
        <p className="text-sm text-zinc-500">
          Full docs:{" "}
          <a
            className="text-emerald-400 hover:underline"
            href="https://github.com/samuelfreemanjobs-hash/Instruments/blob/main/disklordz/daw-inbox/README.md"
          >
            disklordz/daw-inbox/README.md
          </a>
        </p>
      </section>

      <p className="mt-8 text-sm text-zinc-500">
        <Link href="/" className="text-emerald-400 hover:underline">← Back to generator</Link>
      </p>
    </main>
  );
}
