import { createHash } from "crypto";
import { mkdir, readFile, writeFile } from "fs/promises";
import os from "os";
import path from "path";

const ROOT = path.join(os.tmpdir(), "disklordz-kits");

export function kitDirectory(kitId: string): string {
  return path.join(ROOT, kitId);
}

export async function writeKitFile(
  kitId: string,
  filename: string,
  data: Buffer,
): Promise<string> {
  const dir = kitDirectory(kitId);
  await mkdir(dir, { recursive: true });
  const filePath = path.join(dir, filename);
  await writeFile(filePath, data);
  return filePath;
}

export async function readKitFile(kitId: string, filename: string): Promise<Buffer> {
  return readFile(path.join(kitDirectory(kitId), filename));
}

export function sha256Buffer(data: Buffer): string {
  return createHash("sha256").update(data).digest("hex");
}
