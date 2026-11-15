import type { Metadata } from "next";

import { KitGenerator } from "@/components/KitGenerator";

export const metadata: Metadata = {
  title: "Drum Factory",
  description: "Generate original drum one-shots from a prompt and download a WAV pack.",
};

export default function DrumFactoryPage() {
  return (
    <div className="px-4 py-10">
      <KitGenerator />
    </div>
  );
}
