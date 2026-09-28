import type { Metadata } from "next";
import "./globals.css";

export const metadata: Metadata = {
  title: "Junova-X — Juno-class poly synth",
  description:
    "Junova-X: VST3, CLAP, and standalone. Celestial UI, 48 factory presets, host-sync arpeggiator.",
};

export default function RootLayout({ children }: { children: React.ReactNode }) {
  return (
    <html lang="en">
      <body className="antialiased">{children}</body>
    </html>
  );
}
