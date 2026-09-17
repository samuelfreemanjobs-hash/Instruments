import Link from "next/link";
import { ShoppingBag } from "lucide-react";
import { getSession } from "@/lib/auth/session";
import { AccountNav } from "@/components/store/AccountNav";

export async function StoreHeader() {
  const session = await getSession();

  return (
    <header className="border-b border-slate-200">
      <div className="mx-auto flex max-w-6xl items-center justify-between gap-4 px-4 py-4">
        <Link href="/" className="text-lg font-semibold tracking-tight">
          Cursor Shop
        </Link>
        <nav className="flex items-center gap-6 text-sm">
          <Link href="/shop" className="hover:text-slate-600">
            Shop
          </Link>
          <AccountNav session={session} />
          <Link href="/checkout" className="flex items-center gap-1 hover:text-slate-600">
            <ShoppingBag className="h-4 w-4" />
            Cart
          </Link>
          {session?.role === "ADMIN" ? (
            <Link href="/admin" className="text-slate-500 hover:text-slate-800">
              Admin
            </Link>
          ) : null}
        </nav>
      </div>
    </header>
  );
}
