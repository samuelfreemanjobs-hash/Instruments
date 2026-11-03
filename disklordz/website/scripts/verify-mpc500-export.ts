/**
 * MPC500 export contract. Run: npm run verify:mpc500
 */
import { existsSync, mkdtempSync, readdirSync, readFileSync, rmSync } from "fs";
import { tmpdir } from "os";
import { join } from "path";

import { encodeWav } from "../src/lib/generation/wav";
import {
  checkMpc500Wav,
  MPC500_NAME_MAX,
  parseWavHeader,
  planMpc500Kit,
  writeMpc500Kit,
  type LabManifest,
} from "../src/lib/mpc500/kit-export";

const SPRINT_KITS = "../../docs/disklordz/spark-le/sprint-kits";
const KIT_DIRS = readdirSync(join(process.cwd(), SPRINT_KITS), { withFileTypes: true })
  .filter((d) => d.isDirectory() && existsSync(join(process.cwd(), SPRINT_KITS, d.name, "spark-le-manifest.json")))
  .map((d) => `${SPRINT_KITS}/${d.name}`);
if (KIT_DIRS.length === 0) fail(`no kits under ${SPRINT_KITS}`);

function fail(msg: string): never {
  console.error(`verify-mpc500-export: ${msg}`);
  process.exit(1);
}

const factoryWav = parseWavHeader(encodeWav(new Float32Array(441)));
checkMpc500Wav(factoryWav, "factory encodeWav");

const wav24 = encodeWav(new Float32Array(441));
wav24.writeUInt16LE(24, 34);
let rejected = false;
try {
  checkMpc500Wav(parseWavHeader(wav24), "24-bit fixture");
} catch {
  rejected = true;
}
if (!rejected) fail("24-bit WAV was not rejected");

const out = mkdtempSync(join(tmpdir(), "mpc500-"));
try {
  for (const rel of KIT_DIRS) {
    const kitDir = join(process.cwd(), rel);
    const manifest = JSON.parse(readFileSync(join(kitDir, "spark-le-manifest.json"), "utf8")) as LabManifest;
    const kit = planMpc500Kit(kitDir, manifest);
    if (kit.pads.length !== 8) fail(`${rel}: expected 8 core pads, got ${kit.pads.length}`);
    for (const s of [...kit.pads, ...kit.loops]) {
      if (s.sampleName.length > MPC500_NAME_MAX) fail(`${s.sampleName} longer than ${MPC500_NAME_MAX}`);
      if (!/^[A-Z0-9_]+$/.test(s.sampleName)) fail(`${s.sampleName} has characters outside A-Z 0-9 _`);
    }
    const dest = join(out, kit.code);
    writeMpc500Kit(kitDir, dest, kit, manifest);
    for (const s of [...kit.pads, ...kit.loops]) {
      if (!existsSync(join(dest, `${s.sampleName}.WAV`))) fail(`${s.sampleName}.WAV not written`);
    }
    if (!existsSync(join(dest, "PADS.TXT"))) fail("PADS.TXT not written");
  }
} finally {
  rmSync(out, { recursive: true, force: true });
}

console.log(`verify-mpc500-export: OK (${KIT_DIRS.length} kits)`);
