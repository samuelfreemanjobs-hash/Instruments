import { AuthNav } from "@/components/AuthNav";
import { KitGenerator } from "@/components/KitGenerator";

export default function Home() {
  return (
    <div className="min-h-screen bg-zinc-950 px-4 py-12 text-zinc-100">
      <AuthNav />
      <KitGenerator />
      <footer className="mx-auto mt-16 max-w-3xl border-t border-zinc-800 pt-6 text-center text-xs text-zinc-600">
        v0 MVP · parametric factory · deploy via DEPLOY.md ·{" "}
        <a href="/launch" className="text-zinc-400 underline hover:text-zinc-200">
          Memphis launch funnel
        </a>
      </footer>
    </div>
  );
}
