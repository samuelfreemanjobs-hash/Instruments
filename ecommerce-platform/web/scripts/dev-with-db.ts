import { spawn } from "node:child_process";
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
  const uri = mongo.databaseUrl;

  const envLines = [
    `DATABASE_URL="${uri}"`,
    "SESSION_SECRET=dev-local-session-secret-change-in-prod",
    'NEXT_PUBLIC_APP_URL="http://localhost:3000"',
  ];
  fs.writeFileSync(envPath, envLines.join("\n") + "\n");
  console.log("[dev-with-db] Wrote .env.local");

  runPrismaPushSeed(uri);

  const env = { ...process.env, DATABASE_URL: uri };
  const next = spawn("npx", ["next", "dev", "-p", "3000"], {
    cwd: root,
    env,
    stdio: "inherit",
  });

  const shutdown = async () => {
    next.kill("SIGTERM");
    await mongo.stop();
    process.exit(0);
  };
  process.on("SIGINT", shutdown);
  process.on("SIGTERM", shutdown);
  next.on("exit", (code) => {
    void mongo.stop().then(() => process.exit(code ?? 0));
  });
}

main().catch((err) => {
  console.error(err);
  process.exit(1);
});
