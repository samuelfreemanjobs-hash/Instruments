import { TopBarBanner } from "@/components/store/TopBarBanner";
import { StoreChrome } from "@/components/store/StoreChrome";
import { getSession } from "@/lib/auth/session";
import { getCartSnapshot, getFavoriteProducts } from "@/lib/cart/server";

export const dynamic = "force-dynamic";

export default async function StoreLayout({ children }: { children: React.ReactNode }) {
  const [session, cart, favorites] = await Promise.all([
    getSession(),
    getCartSnapshot(),
    getFavoriteProducts(),
  ]);

  return (
    <div className="min-h-screen bg-white text-slate-900">
      <TopBarBanner />
      <StoreChrome session={session} cart={cart} favorites={favorites}>
        {children}
      </StoreChrome>
      <footer className="mt-16 border-t border-slate-200 bg-slate-50 py-8 text-center text-sm text-slate-600">
        © {new Date().getFullYear()} Cursor Shop — built with Next.js 15 + Prisma
      </footer>
    </div>
  );
}
