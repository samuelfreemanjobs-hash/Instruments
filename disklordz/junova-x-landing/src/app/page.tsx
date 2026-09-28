const demoUrl = process.env.NEXT_PUBLIC_DEMO_DOWNLOAD_URL ?? "#download";
const drumSaasUrl = process.env.NEXT_PUBLIC_DISKLORDZ_URL ?? "https://disklordz.com";

export default function HomePage() {
  return (
    <main className="mx-auto max-w-3xl px-6 py-16">
      <p className="text-sm uppercase tracking-widest text-[var(--muted)]">Disklordz Instruments</p>
      <h1 className="mt-3 text-4xl font-semibold tracking-tight md:text-5xl">Junova-X</h1>
      <p className="mt-4 text-lg text-[var(--muted)]">
        Juno-class poly synth for modern DAWs. VST3, CLAP, and standalone. Celestial UI.
      </p>

      <ul className="mt-8 space-y-2 text-[var(--fg)]">
        <li>6-voice Juno mode and 8-voice poly</li>
        <li>BBD-style stereo chorus with Juno-rate LFOs</li>
        <li>Host-sync arpeggiator with latch</li>
        <li>48 factory presets — Linux, macOS, and Windows builds per release</li>
      </ul>

      <div className="mt-10 flex flex-wrap gap-4">
        <a
          id="download"
          href={demoUrl}
          className="rounded-full bg-[var(--accent)] px-6 py-3 text-sm font-medium text-white"
        >
          Download demo
        </a>
        <a
          href={drumSaasUrl}
          className="rounded-full border border-white/20 px-6 py-3 text-sm font-medium"
        >
          Disklordz Drum SaaS
        </a>
      </div>

      <p className="mt-12 text-sm text-[var(--muted)]">
        Launch pricing: $29 early access → $49 standard (Stripe checkout coming soon).
      </p>
    </main>
  );
}
