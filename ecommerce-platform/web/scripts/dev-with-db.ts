import { spawn, spawnSync } from "node:child_process";
import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";
import { MongoMemoryServer } from "mongodb-memory-server";

const root = path.join(path.dirname(fileURLToPath(import.meta.url)), "..");
const envPath = path.join(root, ".env.local");

async function main() {
  const mongod = await MongoMemoryServer.create({
    instance: { dbName: "ecommerce" },
  });
  const uri = mongod.getUri();

  const envLines = [
    `DATABASE_URL="${uri}"`,
    "SESSION_SECRET=dev-local-session-secret-change-in-prod",
    'NEXT_PUBLIC_APP_URL="http://localhost:3000"',
  ];
  fs.writeFileSync(envPath, envLines.join("\n") + "\n");
  console.log("[dev-with-db] Wrote .env.local with in-memory MongoDB");

  const env = { ...process.env, DATABASE_URL: uri };
  const push = spawnSync("npx", ["prisma", "db", "push"], { cwd: root, env, stdio: "inherit" });
  if (push.status !== 0) {
    await mongod.stop();
    process.exit(push.status ?? 1);
  }

  const seed = spawnSync("npx", ["tsx", "prisma/seed.ts"], { cwd: root, env, stdio: "inherit" });
  if (seed.status !== 0) {
    await mongod.stop();
    process.exit(seed.status ?? 1);
  }

  const next = spawn("npx", ["next", "dev", "-p", "3000"], {
    cwd: root,
    env,
    stdio: "inherit",
  });

  const shutdown = async () => {
    next.kill("SIGTERM");
    await mongod.stop();
    process.exit(0);
  };
  process.on("SIGINT", shutdown);
  process.on("SIGTERM", shutdown);
  next.on("exit", (code) => {
    void mongod.stop().then(() => process.exit(code ?? 0));
  });
}

main().catch((err) => {
  console.error(err);
  process.exit(1);
});
