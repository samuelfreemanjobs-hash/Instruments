#!/usr/bin/env node
/** Preset ↔ PRESET_BASE alignment (WO-SAAS-023). */
import { readFileSync } from "fs";
import { dirname, join } from "path";
import { fileURLToPath } from "url";

const root = join(dirname(fileURLToPath(import.meta.url)), "../../..");
const presetsPath = join(root, "disklordz/website/src/lib/presets.ts");
const paramsPath = join(root, "disklordz/website/src/lib/generation/prompt-params.ts");

const presetsSrc = readFileSync(presetsPath, "utf8");
const paramsSrc = readFileSync(paramsPath, "utf8");

const presetIds = [...presetsSrc.matchAll(/id:\s*"([^"]+)"/g)].map((m) => m[1]);
const baseIds = [...paramsSrc.matchAll(/"([a-z0-9-]+)":\s*\{/g)]
  .map((m) => m[1])
  .filter((id) => id.includes("-"));

const missingInBase = presetIds.filter((id) => !baseIds.includes(id));
const extraInBase = baseIds.filter((id) => !presetIds.includes(id));

if (missingInBase.length) {
  console.error("FAIL: presets missing PRESET_BASE:", missingInBase.join(", "));
  process.exit(1);
}
if (extraInBase.length) {
  console.error("FAIL: PRESET_BASE ids not in STYLE_PRESETS:", extraInBase.join(", "));
  process.exit(1);
}

console.log("OK: preset-lane check", { presetCount: presetIds.length, ids: presetIds });
