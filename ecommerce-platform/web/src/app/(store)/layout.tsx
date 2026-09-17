import Link from "next/link";
import { ShoppingBag, User } from "lucide-react";

export default function StoreLayout({ children }: { children: React.ReactNode }) {
  return (
    <div className="min-h-screen bg-white text-slate-900">
      <div className="bg-slate-900 px-4 py-2 text-center text-sm text-white">
        Free shipping on orders over $50 —{" "}
        <span className="underline">TopBar from CMS (SOP-05)</span>
      </div>
      <header className="border-b border-slate-200">
        <div className="mx-auto flex max-w-6xl items-center justify-between gap-4 px-4 py-4">
          <Link href="/" className="text-lg font-semibold tracking-tight">
            Cursor Shop
          </Link>
          <nav className="hidden items-center gap-6 text-sm md:flex">
            <Link href="/shop" className="hover:text-slate-600">
              Shop
            </Link>
            <Link href="/auth/login" className="flex items-center gap-1 hover:text-slate-600">
              <User className="h-4 w-4" />
              Account
            </Link>
            <Link href="/checkout" className="flex items-center gap-1 hover:text-slate-600">
              <ShoppingBag className="h-4 w-4" />
              Cart
            </Link>
          </nav>
          <Link
            href="/admin"
            className="text-xs text-slate-500 hover:text-slate-800 md:text-sm"
          >
            Admin
          </Link>
        </div>
      </header>
      <main>{children}</main>
      <footer className="mt-16 border-t border-slate-200 bg-slate-50 py-8 text-center text-sm text-slate-600">
        © {new Date().getFullYear()} Cursor Shop — built with Next.js 15 + Prisma
      </footer>
    </div>
  );
}
