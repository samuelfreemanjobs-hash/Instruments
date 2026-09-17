import { KitGenerator } from "@/components/KitGenerator";

export default function Home() {
  return (
    <div className="min-h-screen bg-zinc-950 px-4 py-12 text-zinc-100">
      <KitGenerator />
      <footer className="mx-auto mt-16 max-w-3xl border-t border-zinc-800 pt-6 text-center text-xs text-zinc-600">
        v0 stub generation — WO-SAAS-001–005 · Auth (WO-002) and live factory (WO-004) next
      </footer>
    </div>
  );
}
