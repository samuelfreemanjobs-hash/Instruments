import { NextResponse } from "next/server";

import { readKitFile } from "@/lib/kit-storage";

const ALLOWED = new Set([
  "kick.wav",
  "snare.wav",
  "hat_closed.wav",
  "hat_open.wav",
  "rim.wav",
  "clap.wav",
]);

export async function GET(
  _req: Request,
  context: { params: Promise<{ kitId: string; filename: string }> },
) {
  const { kitId, filename } = await context.params;
  if (!kitId || !ALLOWED.has(filename)) {
    return NextResponse.json({ error: "not_found" }, { status: 404 });
  }

  try {
    const data = await readKitFile(kitId, filename);
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
