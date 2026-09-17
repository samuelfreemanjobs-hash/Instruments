import fs from "node:fs";
import path from "node:path";
import {
  ensureMongoForPrisma,
  runPrismaPushSeed,
  webRoot,
} from "./lib/ensure-mongodb";

const root = webRoot;
const envPath = path.join(root, ".env.local");

async function main() {
  const mongo = await ensureMongoForPrisma();

  const preserveSecret = fs.existsSync(envPath)
    ? fs
        .readFileSync(envPath, "utf8")
        .split("\n")
        .find((l) => l.startsWith("SESSION_SECRET="))
    : null;

  const envLines = [
    `DATABASE_URL="${mongo.databaseUrl}"`,
    preserveSecret ?? "SESSION_SECRET=dev-local-session-secret-change-in-prod",
    'NEXT_PUBLIC_APP_URL="http://localhost:3000"',
  ];
  fs.writeFileSync(envPath, envLines.join("\n") + "\n");
  console.log("[bootstrap-local-db] Wrote .env.local");

  runPrismaPushSeed(mongo.databaseUrl);
  console.log("[bootstrap-local-db] Ready:", mongo.databaseUrl);
  console.log("[bootstrap-local-db] Keep mongod running (memory server stops when this process exits).");
}

main().catch((e) => {
  console.error(e);
  process.exit(1);
});
