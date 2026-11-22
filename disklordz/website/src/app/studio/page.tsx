import { KitGenerator } from "@/components/KitGenerator";

export const metadata = {
  title: "Prompt studio · DiskLordz",
  description: "Prompt-driven drum kit generation with presets, variations, and product packs.",
};

export default function StudioPage() {
  return (
    <div className="studio-wrap">
      <KitGenerator />
      <footer className="page-footer">
        Prompt path · parametric factory · credits apply when signed in
      </footer>
    </div>
  );
}
