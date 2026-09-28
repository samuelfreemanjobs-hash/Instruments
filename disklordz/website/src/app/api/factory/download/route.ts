import JSZip from "jszip";
import { NextRequest, NextResponse } from "next/server";

import { readKitFile } from "@/lib/kit-storage";
import { sha256Buffer } from "@/lib/kit-store";
import type { ProductPackManifest } from "@/lib/manifest";
import { getPreset } from "@/lib/presets";

export async function POST(req: NextRequest) {
  let manifest: ProductPackManifest;
  try {
    const body = await req.json();
    manifest = body.productPack as ProductPackManifest;
  } catch {
    return NextResponse.json({ error: "invalid_json" }, { status: 400 });
  }

  if (
    manifest?.format !== "DISKLORDZ_PRODUCT_PACK_MANIFEST" ||
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
    "Disklordz Product Pack (WO-SAAS-015 preview)",
    `Brief: ${manifest.prompt}`,
    `Preset: ${manifest.presetId}`,
    `Pack ID: ${manifest.packId}`,
    "",
    "Folder layout matches storefront SKU template (01_KICKS … 04_PERC).",
    manifest.license,
  ].join("\n"));

  for (const sample of manifest.samples) {
    let data: Buffer;
    try {
      data = await readKitFile(manifest.packId, sample.filename);
    } catch {
      return NextResponse.json({ error: "pack_expired", sample: sample.name }, { status: 410 });
    }
    const hash = sha256Buffer(data);
    if (hash !== sample.sha256) {
      return NextResponse.json({ error: "provenance_mismatch", sample: sample.name }, { status: 409 });
    }
    zip.file(sample.filename, data);
  }

  const zipBuffer = await zip.generateAsync({ type: "nodebuffer", compression: "DEFLATE" });
  const filename = `disklordz-pack-${manifest.presetId}-${manifest.packId.slice(0, 8)}.zip`;

  return new NextResponse(new Uint8Array(zipBuffer), {
    status: 200,
    headers: {
      "Content-Type": "application/zip",
      "Content-Disposition": `attachment; filename="${filename}"`,
    },
  });
}
