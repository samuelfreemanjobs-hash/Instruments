import type { Metadata } from "next";

export const metadata: Metadata = {
  title: "License",
  description: "Memphis Vault license status. Store policies are not published yet.",
};

export default function LegalPage() {
  return (
    <div className="mx-auto max-w-3xl px-4 py-12">
      <h1 className="font-display text-5xl text-ivory">License</h1>
      <p className="mt-4 text-ivory/80">
        Memphis Vault Vol. 1 is original DiskLordz synthesis. It is not sampled from records, machines, or
        third-party kits. The store license text is not final, and the pack is not for sale.
      </p>
      <p className="mt-3 text-ivory/80">Refund, privacy, and terms pages are not published yet.</p>
    </div>
  );
}
