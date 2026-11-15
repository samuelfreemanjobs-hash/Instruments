import { readFile } from "node:fs/promises";
import path from "node:path";

import { MEMPHIS_VAULT_PREVIEWS, memphisVaultDir } from "@/lib/catalog/memphis-vault";

const ALLOWED = new Set<string>(["cover.jpg", ...MEMPHIS_VAULT_PREVIEWS]);

export async function GET(request: Request) {
  const name = new URL(request.url).searchParams.get("file") ?? "";
  if (!ALLOWED.has(name)) {
    return new Response("Not found", { status: 404 });
  }

  const root = memphisVaultDir();
  const filePath = path.resolve(root, name);
  const relative = path.relative(root, filePath);
  if (relative.startsWith("..") || path.isAbsolute(relative)) {
    return new Response("Not found", { status: 404 });
  }

  try {
    const bytes = await readFile(filePath);
    const isWav = name.endsWith(".wav");
    return new Response(bytes, {
      headers: {
        "Content-Type": isWav ? "audio/wav" : "image/jpeg",
        "Cache-Control": "public, max-age=3600",
        "X-Content-Type-Options": "nosniff",
      },
    });
  } catch {
    return new Response("Not found", { status: 404 });
  }
}
