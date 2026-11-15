/**
 * Write MPC500 card folders for all sprint kits under each kit's MPC500/<code>/.
 * Run: npm run export:mpc500-sprint-all
 */
import { existsSync, readdirSync, readFileSync } from "fs";
import { join, resolve } from "path";

import { planMpc500Kit, writeMpc500Kit, type LabManifest } from "../src/lib/mpc500/kit-export";

const SPRINT = resolve(process.cwd(), "../../docs/disklordz/spark-le/sprint-kits");

/** Stable short codes (unique across kits); see MPC500-CARD-EXPORT.md */
const KIT_CODES: Record<string, string> = {
  "session-1-gangsta-trap": "TOD140",
  "session-2-dj-screw": "DS72",
  "session-3-smooth-jazz-rap": "SJR86",
  "session-4-trunk-sermon": "FP84",
  "session-5-stretch-uk-drill": "UD144",
};

const dirs = readdirSync(SPRINT, { withFileTypes: true })
  .filter((d) => d.isDirectory())
  .map((d) => d.name)
  .filter((name) => existsSync(join(SPRINT, name, "spark-le-manifest.json")))
  .sort();

if (dirs.length === 0) {
  console.error("export-mpc500-sprint-all: no sprint kits found");
  process.exit(1);
}

let totalMb = 0;
for (const name of dirs) {
  const kitDir = join(SPRINT, name);
  const code = KIT_CODES[name];
  if (!code) {
    console.error(`export-mpc500-sprint-all: no code mapping for ${name}`);
    process.exit(1);
  }
  const manifest = JSON.parse(readFileSync(join(kitDir, "spark-le-manifest.json"), "utf8")) as LabManifest;
  const kit = planMpc500Kit(kitDir, manifest, { code });
  const outDir = join(kitDir, "MPC500", kit.code);
  writeMpc500Kit(kitDir, outDir, kit, manifest);
  totalMb += kit.totalBytes / 1048576;
  for (const w of kit.warnings) console.warn(`export-mpc500-sprint-all: ${name}: ${w}`);
  console.log(`export-mpc500-sprint-all: ${name} → ${outDir} (${(kit.totalBytes / 1048576).toFixed(2)} MB)`);
}

console.log(`export-mpc500-sprint-all: OK (${dirs.length} kits, ${totalMb.toFixed(2)} MB total)`);
