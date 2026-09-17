import JSZip from "jszip";
import { NextRequest, NextResponse } from "next/server";

import type { KitManifest } from "@/lib/manifest";
import { readKitFile, sha256Buffer } from "@/lib/kit-store";
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
    let data: Buffer;
    try {
      data = await readKitFile(manifest.kitId, sample.filename);
    } catch {
      return NextResponse.json({ error: "kit_expired", sample: sample.name }, { status: 410 });
    }
    const hash = sha256Buffer(data);
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
