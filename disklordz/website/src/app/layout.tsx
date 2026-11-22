import type { Metadata } from "next";
import { IBM_Plex_Mono, Syne } from "next/font/google";

import { SiteChrome } from "@/components/SiteChrome";

import "./globals.css";

const syne = Syne({
  variable: "--font-display",
  subsets: ["latin"],
  weight: ["500", "600", "700", "800"],
});

const plexMono = IBM_Plex_Mono({
  variable: "--font-mono",
  subsets: ["latin"],
  weight: ["400", "500", "600"],
});

export const metadata: Metadata = {
  title: "DiskLordz Drum Kit Factory",
  description:
    "Formula → audition → export. Original drum kit packs from 32 DNA formulas — preview pads and download MPC-ready WAVs.",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en" className="dark">
      <body className={`${syne.variable} ${plexMono.variable} antialiased`}>
        <div className="page-atmosphere">
          <SiteChrome />
          {children}
        </div>
      </body>
    </html>
  );
}
