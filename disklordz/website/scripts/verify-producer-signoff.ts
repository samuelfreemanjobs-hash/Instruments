/**
 * Gate for "808 Lab sprint fully finished" (human MPC pass).
 * Passes when all five sprint reviews have scores and Session 5 catalog note.
 * Run: npm run verify:producer-signoff
 */
import { readFileSync } from "fs";
import { join } from "path";

const REVIEWS = [
  "2026-11-03_TRAP-OR-DIE_sprint1.md",
  "2026-11-03_DJ-SCREW_sprint2.md",
  "2026-11-03_SMOOTH-JAZZ-RAP_sprint3.md",
  "2026-11-03_TRUNK-SERMON_sprint4.md",
  "2026-11-03_UK-DRILL_sprint5.md",
];

const REV_DIR = join(process.cwd(), "../../docs/disklordz/spark-le/reviews");

function fail(msg: string): never {
  console.error(`verify-producer-signoff: ${msg}`);
  process.exit(1);
}

function scoreFilled(md: string, axis: string): boolean {
  const re = new RegExp(`\\|\\s*\\*\\*${axis}\\*\\*\\s*\\|\\s*([0-9]|\\d\\.\\d)`, "i");
  return re.test(md);
}

let keepers = 0;
for (const file of REVIEWS) {
  const path = join(REV_DIR, file);
  let md: string;
  try {
    md = readFileSync(path, "utf8");
  } catch {
    fail(`missing review ${file}`);
  }
  for (const axis of ["Sonic", "Groove", "Workflow", "Vibe"]) {
    if (!scoreFilled(md, axis)) fail(`${file}: fill **${axis}** score (1–5)`);
  }
  if (/\|\s*Y\s*\|/i.test(md) || /\|\s*session-\d[^\|]*\|\s*Y\s*\|/i.test(md)) keepers += 1;
}

if (keepers < 3) {
  fail(`need ≥3 keepers (Y) in review tables; found ${keepers}`);
}

const s5 = readFileSync(join(REV_DIR, REVIEWS[4]!), "utf8");
if (!/catalog/i.test(s5) || !/(keep|drop)/i.test(s5)) {
  fail("session 5 review: add catalog keep/drop paragraph (mention keep or drop)");
}

console.log(`verify-producer-signoff: OK (${keepers} keepers flagged)`);
