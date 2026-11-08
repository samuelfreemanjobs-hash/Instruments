import type { Metadata, Viewport } from "next";
import { ShellNav } from "@/components/ShellNav";
import { AuthStatus } from "@/components/AuthStatus";
import { publicRuntimeConfig } from "@/lib/public-env";

import "./globals.css";

export const metadata: Metadata = {
  title: "Black Friday Madness",
  description: "AI shopping intelligence — real savings vs history, not retailer discount ads.",
  applicationName: "Black Friday Madness",
};

export const viewport: Viewport = {
  themeColor: "#080A0F",
  width: "device-width",
  initialScale: 1,
};

export default function RootLayout({ children }: { children: React.ReactNode }) {
  const publicCfg = publicRuntimeConfig();
  const configJson = JSON.stringify(publicCfg).replace(/</g, "\\u003c");
  return (
    <html lang="en">
      <body className="min-h-dvh antialiased">
        <script id="bfm-public-config" type="application/json" dangerouslySetInnerHTML={{ __html: configJson }} />
        <header className="border-b border-bfm-border px-4 py-4">
          <div className="mx-auto flex max-w-5xl flex-col gap-2 sm:flex-row sm:items-end sm:justify-between">
            <div>
              <p className="text-xs uppercase tracking-widest text-bfm-muted">Year-round · peaks at Black Friday</p>
              <h1 className="text-xl font-bold tracking-tight sm:text-2xl">
                BLACK FRIDAY <span className="text-bfm-accent">MADNESS</span>
              </h1>
            </div>
            <AuthStatus />
          </div>
        </header>
        <ShellNav />
        <main className="mx-auto max-w-5xl px-4 py-6">{children}</main>
      </body>
    </html>
  );
}
