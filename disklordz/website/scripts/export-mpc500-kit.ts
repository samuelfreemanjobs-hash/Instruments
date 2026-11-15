/**
 * Export an 808 Lab kit folder as an Akai MPC500 card folder.
 *
 * Usage:
 *   npx tsx scripts/export-mpc500-kit.ts \
 *     --kit ../../docs/disklordz/spark-le/sprint-kits/session-3-smooth-jazz-rap \
 *     --out E:\DISKLORDZ\SJR86 \
 *     [--code SJR86] [--ram-mb 16]
 *
 * --out defaults to <kit>/MPC500/<code>.
 */
import { readFileSync } from "fs";
import { join, resolve } from "path";

import { planMpc500Kit, writeMpc500Kit, type LabManifest } from "../src/lib/mpc500/kit-export";

const argv = process.argv.slice(2);
const arg = (flag: string): string | undefined => {
  const i = argv.indexOf(flag);
  return i >= 0 ? argv[i + 1] : undefined;
};

const kitArg = arg("--kit");
if (!kitArg) {
  console.error("Usage: export-mpc500-kit.ts --kit <kit folder> [--out <dir>] [--code <name>] [--ram-mb <n>]");
  process.exit(1);
}

const kitDir = resolve(kitArg);
const manifest = JSON.parse(readFileSync(join(kitDir, "spark-le-manifest.json"), "utf8")) as LabManifest;
const ramMb = arg("--ram-mb") ? Number(arg("--ram-mb")) : undefined;

try {
  const kit = planMpc500Kit(kitDir, manifest, { code: arg("--code"), ramMb });
  const outDir = resolve(arg("--out") ?? join(kitDir, "MPC500", kit.code));
  writeMpc500Kit(kitDir, outDir, kit, manifest);
  for (const w of kit.warnings) console.warn(`export-mpc500-kit: warning: ${w}`);
  console.log(
    `export-mpc500-kit: OK → ${outDir} (${kit.pads.length} pads, ${kit.loops.length} loops, ${(kit.totalBytes / 1048576).toFixed(2)} MB)`,
  );
} catch (e) {
  console.error(`export-mpc500-kit: ${(e as Error).message}`);
  process.exit(1);
}
