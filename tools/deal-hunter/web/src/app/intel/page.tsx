import { IntelClient } from "./intel-client";

export default function IntelPage() {
  return (
    <section className="space-y-4">
      <header className="space-y-1">
        <h2 className="text-lg font-semibold">Deal intelligence</h2>
        <p className="text-sm text-bfm-muted">Explainable 0–100 scores and category trends.</p>
      </header>
      <IntelClient />
    </section>
  );
}
