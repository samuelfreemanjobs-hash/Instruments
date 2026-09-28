import { NextResponse } from "next/server";

import { readKitFile } from "@/lib/kit-storage";

const ALLOWED_BASENAMES = new Set([
  "kick.wav",
  "snare.wav",
  "hat_closed.wav",
  "hat_open.wav",
  "rim.wav",
  "clap.wav",
  "loop_main.wav",
  "sfx.wav",
]);

const PACK_FOLDER = /^0[1-4]_[A-Z]+$/;

function isAllowedRelativePath(parts: string[]): boolean {
  if (parts.length === 1) {
    return ALLOWED_BASENAMES.has(parts[0]);
  }
  if (parts.length === 2 && PACK_FOLDER.test(parts[0])) {
    return /^[a-z_]+_\d{2}\.wav$/.test(parts[1]);
  }
  return false;
}

export async function GET(
  _req: Request,
  context: { params: Promise<{ kitId: string; filename: string[] }> },
) {
  const { kitId, filename: parts } = await context.params;
  if (!kitId || !parts?.length || !isAllowedRelativePath(parts)) {
    return NextResponse.json({ error: "not_found" }, { status: 404 });
  }

  const relativePath = parts.join("/");

  try {
    const data = await readKitFile(kitId, relativePath);
    return new NextResponse(new Uint8Array(data), {
      headers: {
        "Content-Type": "audio/wav",
        "Cache-Control": "private, max-age=3600",
      },
    });
  } catch {
    return NextResponse.json({ error: "not_found" }, { status: 404 });
  }
}
