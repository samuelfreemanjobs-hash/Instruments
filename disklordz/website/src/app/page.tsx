import Link from "next/link";

import { KitFactoryLab } from "@/components/KitFactoryLab";

export default function Home() {
  return (
    <>
      <section className="hero" aria-label="DiskLordz Drum Kit Factory">
        <div className="hero-inner">
          <p className="hero-brand">DiskLordz</p>
          <h1 className="hero-line">Drum Kit Factory</h1>
          <p className="hero-sub">
            Pick a DNA formula, audition all 16 pads, export a sellable WAV pack. Original synthesis —
            no third-party sample libraries.
          </p>
          <div className="hero-cta">
            <a className="btn-primary" href="#factory">
              Open factory
            </a>
            <Link className="btn-secondary" href="/studio">
              Prompt studio
            </Link>
          </div>
        </div>
      </section>

      <section id="factory" className="factory-section">
        <h2 className="section-title">Build pack</h2>
        <p className="section-sub">
          Kit Formula is the primary control. Producer engines stay optional in Prompt studio.
        </p>
        <KitFactoryLab />
      </section>

      <footer className="page-footer">
        DiskLordz Supply · formula → pads → export ·{" "}
        <Link href="/daw-inbox" className="text-[var(--dl-amber)] hover:underline">
          DAW inbox
        </Link>
      </footer>
    </>
  );
}
