"use client";

import { useCallback, useMemo, useRef, useState } from "react";

import {
  DNA_FORMULAS,
  PAD_NAMES,
  formulaAt,
  padSampleHint,
  type DnaFormula,
} from "@/lib/dna-formulas";
import { defaultGenerationSpec } from "@/lib/generation/generation-spec";
import { PRODUCT_PACK_CREDIT_COST } from "@/lib/generation/mode-utils";
import type { KitManifest, ProductPackManifest } from "@/lib/manifest";
import { auditionPad } from "@/lib/preview/pad-preview";

type GenerateResponse = {
  batchId: string;
  variations: { label: string; manifest: KitManifest }[];
  manifest?: KitManifest;
  billing?: {
    plan: string;
    creditsBalance: number;
    unlimited: boolean;
  };
};

export function KitFactoryLab() {
  const [formulaIndex, setFormulaIndex] = useState(8);
  const [selectedPad, setSelectedPad] = useState(0);
  const [variations, setVariations] = useState(4);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [status, setStatus] = useState("Pick a formula. Tap pads to audition. Export a pack.");
  const [kit, setKit] = useState<KitManifest | null>(null);
  const [productPack, setProductPack] = useState<ProductPackManifest | null>(null);
  const [playingPad, setPlayingPad] = useState<number | null>(null);
  const audioRef = useRef<HTMLAudioElement | null>(null);

  const formula = useMemo(() => formulaAt(formulaIndex), [formulaIndex]);

  const formulasByLine = useMemo(() => {
    const groups = new Map<string, DnaFormula[]>();
    for (const f of DNA_FORMULAS) {
      const list = groups.get(f.line) ?? [];
      list.push(f);
      groups.set(f.line, list);
    }
    return groups;
  }, []);

  const hitPad = useCallback(
    async (padIndex: number) => {
      setSelectedPad(padIndex);
      setPlayingPad(padIndex);
      setError(null);

      const sample = kit?.samples.find((s) =>
        s.name.toLowerCase().includes(padSampleHint(padIndex)),
      );
      if (sample?.url) {
        if (audioRef.current) audioRef.current.pause();
        const audio = new Audio(sample.url);
        audioRef.current = audio;
        try {
          await audio.play();
          audio.onended = () => setPlayingPad(null);
        } catch {
          setPlayingPad(null);
        }
        return;
      }

      try {
        await auditionPad(padIndex, { formulaIndex, velocity: 0.9 });
      } finally {
        window.setTimeout(() => setPlayingPad(null), 220);
      }
    },
    [formulaIndex, kit],
  );

  const buildKit = useCallback(async () => {
    setLoading(true);
    setError(null);
    setProductPack(null);
    setStatus(`Building ${formula.displayName}…`);
    try {
      const spec = {
        ...defaultGenerationSpec(formula.presetId),
        mode: "one_shot" as const,
        engine: "studio" as const,
      };
      const res = await fetch("/api/generate", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          prompt: `${formula.promptSeed} · DNA ${formula.id} · ${variations} variations`,
          presetId: formula.presetId,
          spec,
        }),
      });
      const data = (await res.json()) as GenerateResponse & {
        message?: string;
        error?: string;
      };
      if (!res.ok) {
        setError(data.message ?? data.error ?? "Generation failed");
        setStatus("Build failed.");
        return;
      }
      const manifest =
        data.variations?.[0]?.manifest ?? data.manifest ?? null;
      if (!manifest) {
        setError("No kit returned.");
        return;
      }
      setKit(manifest);
      setStatus(
        `Ready · ${manifest.samples.length} one-shots · batch ${(data.batchId ?? "").slice(0, 8)}`,
      );
    } catch {
      setError("Network error — try again.");
      setStatus("Build failed.");
    } finally {
      setLoading(false);
    }
  }, [formula, variations]);

  const buildPack = useCallback(async () => {
    setLoading(true);
    setError(null);
    setStatus(`Building storefront pack (${PRODUCT_PACK_CREDIT_COST} credits)…`);
    try {
      const spec = {
        ...defaultGenerationSpec(formula.presetId),
        mode: "one_shot" as const,
        engine: "studio" as const,
      };
      const res = await fetch("/api/factory/batch", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          prompt: `${formula.promptSeed} · DNA ${formula.id} product pack`,
          presetId: formula.presetId,
          spec,
        }),
      });
      const data = await res.json();
      if (!res.ok) {
        setError(data.message ?? data.error ?? "Product pack failed");
        setStatus("Pack failed.");
        return;
      }
      setProductPack(data.productPack as ProductPackManifest);
      setStatus(
        `Pack ready · ${(data.productPack as ProductPackManifest).samples.length} WAVs`,
      );
    } catch {
      setError("Product pack network error.");
      setStatus("Pack failed.");
    } finally {
      setLoading(false);
    }
  }, [formula]);

  const downloadKitZip = useCallback(async () => {
    if (!kit) return;
    setLoading(true);
    setError(null);
    try {
      const res = await fetch("/api/download", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ manifest: kit }),
      });
      if (!res.ok) {
        const data = await res.json();
        setError(data.error ?? "Download failed");
        return;
      }
      const blob = await res.blob();
      const url = URL.createObjectURL(blob);
      const a = document.createElement("a");
      a.href = url;
      a.download = `disklordz-${formula.id}.zip`;
      a.click();
      URL.revokeObjectURL(url);
      setStatus("Kit ZIP downloaded.");
    } catch {
      setError("Download failed.");
    } finally {
      setLoading(false);
    }
  }, [kit, formula.id]);

  const downloadPackZip = useCallback(async () => {
    if (!productPack) return;
    setLoading(true);
    setError(null);
    try {
      const res = await fetch("/api/factory/download", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ productPack }),
      });
      if (!res.ok) {
        const data = await res.json();
        setError(data.error ?? "Pack download failed");
        return;
      }
      const blob = await res.blob();
      const url = URL.createObjectURL(blob);
      const a = document.createElement("a");
      a.href = url;
      a.download = `disklordz-pack-${formula.id}.zip`;
      a.click();
      URL.revokeObjectURL(url);
      setStatus("Product pack ZIP downloaded.");
    } catch {
      setError("Pack download failed.");
    } finally {
      setLoading(false);
    }
  }, [productPack, formula.id]);

  return (
    <div className="factory-shell">
      <section className="factory-controls" aria-label="Kit formula">
        <div className="factory-field">
          <label htmlFor="dna-formula">Kit Formula</label>
          <select
            id="dna-formula"
            value={formulaIndex}
            onChange={(e) => {
              const next = Number(e.target.value);
              setFormulaIndex(next);
              setKit(null);
              setProductPack(null);
              setStatus(`${formulaAt(next).displayName} loaded — audition pads or build.`);
            }}
          >
            {[...formulasByLine.entries()].map(([line, list]) => (
              <optgroup key={line} label={line}>
                {list.map((f) => (
                  <option key={f.id} value={f.index}>
                    {f.displayName}
                  </option>
                ))}
              </optgroup>
            ))}
          </select>
        </div>

        <div className="factory-meta">
          <p>
            <span>Line</span> {formula.line}
          </p>
          <p>
            <span>Engine lane</span> {formula.presetId}
          </p>
          <p>
            <span>ID</span> {formula.id}
          </p>
        </div>

        <div className="factory-field">
          <label htmlFor="variations">Variations</label>
          <input
            id="variations"
            type="range"
            min={1}
            max={8}
            step={1}
            value={variations}
            onChange={(e) => setVariations(Number(e.target.value))}
          />
          <span className="factory-range-value">{variations}</span>
        </div>

        <div className="factory-actions">
          <button type="button" className="btn-primary" disabled={loading} onClick={buildKit}>
            {loading ? "Working…" : "Build kit"}
          </button>
          <button type="button" className="btn-secondary" disabled={loading || !kit} onClick={downloadKitZip}>
            Export kit ZIP
          </button>
          <button type="button" className="btn-secondary" disabled={loading} onClick={buildPack}>
            Build product pack
          </button>
          <button
            type="button"
            className="btn-secondary"
            disabled={loading || !productPack}
            onClick={downloadPackZip}
          >
            Export pack ZIP
          </button>
        </div>

        <p className="factory-status" role="status">
          {status}
        </p>
        {error && <p className="factory-error">{error}</p>}
      </section>

      <section className="factory-pads" aria-label="16 pad bank">
        <div className="factory-pads-head">
          <h2>16 pads</h2>
          <p>
            Selected: <strong>{PAD_NAMES[selectedPad]}</strong>
            {kit ? " · live WAV" : " · synth preview"}
          </p>
        </div>
        <div className="pad-grid">
          {PAD_NAMES.map((name, i) => (
            <button
              key={name}
              type="button"
              className={[
                "pad",
                selectedPad === i ? "is-selected" : "",
                playingPad === i ? "is-playing" : "",
              ]
                .filter(Boolean)
                .join(" ")}
              onClick={() => void hitPad(i)}
            >
              <span className="pad-index">{String(i + 1).padStart(2, "0")}</span>
              <span className="pad-name">{name}</span>
            </button>
          ))}
        </div>
      </section>
    </div>
  );
}
