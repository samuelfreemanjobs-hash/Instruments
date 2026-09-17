import { TopBarBanner } from "@/components/store/TopBarBanner";
import { StoreHeader } from "@/components/store/StoreHeader";

export const dynamic = "force-dynamic";

export default function StoreLayout({ children }: { children: React.ReactNode }) {
  return (
    <div className="min-h-screen bg-white text-slate-900">
      <TopBarBanner />
      <StoreHeader />
      <main>{children}</main>
      <footer className="mt-16 border-t border-slate-200 bg-slate-50 py-8 text-center text-sm text-slate-600">
        © {new Date().getFullYear()} Cursor Shop — built with Next.js 15 + Prisma
      </footer>
    </div>
  );
}
