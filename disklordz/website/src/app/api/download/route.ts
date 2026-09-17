import { createHash } from "crypto";
import { readFile } from "fs/promises";
import path from "path";

import JSZip from "jszip";
import { NextRequest, NextResponse } from "next/server";

import type { KitManifest } from "@/lib/manifest";
import { getPreset } from "@/lib/presets";

export async function POST(req: NextRequest) {
  let manifest: KitManifest;
  try {
    const body = await req.json();
    manifest = body.manifest as KitManifest;
  } catch {
    return NextResponse.json({ error: "invalid_json" }, { status: 400 });
  }

  if (
    manifest?.format !== "DISKLORDZ_DRUM_KIT_MANIFEST" ||
    !manifest.presetId ||
    !Array.isArray(manifest.samples)
  ) {
    return NextResponse.json({ error: "invalid_manifest" }, { status: 400 });
  }

  if (!getPreset(manifest.presetId)) {
    return NextResponse.json({ error: "invalid_preset" }, { status: 400 });
  }

  const zip = new JSZip();
  zip.file("manifest.json", JSON.stringify(manifest, null, 2));
  zip.file("README.txt", [
    "Disklordz Drum Kit (v0 preview)",
    `Prompt: ${manifest.prompt}`,
    `Preset: ${manifest.presetId}`,
    `Kit ID: ${manifest.kitId}`,
    "",
    "44.1 kHz mono WAV — MPC-ready naming.",
    manifest.license,
  ].join("\n"));

  for (const sample of manifest.samples) {
    const filePath = path.join(
      process.cwd(),
      "public",
      "samples",
      manifest.presetId,
      sample.filename,
    );
    const data = await readFile(filePath);
    const hash = createHash("sha256").update(data).digest("hex");
    if (hash !== sample.sha256) {
      return NextResponse.json({ error: "provenance_mismatch", sample: sample.name }, { status: 409 });
    }
    zip.file(sample.filename, data);
  }

  const zipBuffer = await zip.generateAsync({ type: "nodebuffer", compression: "DEFLATE" });
  const filename = `disklordz-${manifest.presetId}-${manifest.kitId.slice(0, 8)}.zip`;

  return new NextResponse(new Uint8Array(zipBuffer), {
    status: 200,
    headers: {
      "Content-Type": "application/zip",
      "Content-Disposition": `attachment; filename="${filename}"`,
    },
  });
}
