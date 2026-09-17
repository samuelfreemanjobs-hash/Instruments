import { spawn, spawnSync, type ChildProcessWithoutNullStreams } from "node:child_process";
import fs from "node:fs";
import path from "node:path";
import { MongoMemoryServer } from "mongodb-memory-server";
import { PrismaClient } from "@prisma/client";
import { fileURLToPath } from "node:url";

const scriptsDir = path.dirname(fileURLToPath(import.meta.url));
export const webRoot = path.join(scriptsDir, "..");

export type MongoHandle = {
  databaseUrl: string;
  stop: () => Promise<void>;
};

/** Prisma on MongoDB always uses transactions; FerretDB/DocumentDB is not sufficient. */
export async function probePrismaWrites(databaseUrl: string): Promise<boolean> {
  const prisma = new PrismaClient({
    datasources: { db: { url: databaseUrl } },
  });
  const email = `probe_${Date.now()}@local.test`;
  try {
    await prisma.user.create({
      data: {
        email,
        name: "Probe",
        passwordHash: "probe",
        role: "CUSTOMER",
      },
    });
    await prisma.user.delete({ where: { email } });
    return true;
  } catch {
    return false;
  } finally {
    await prisma.$disconnect();
  }
}

async function startMemoryServer(): Promise<MongoHandle> {
  console.log("[ensure-mongodb] Starting mongodb-memory-server (downloads from MongoDB CDN)…");
  const mongod = await MongoMemoryServer.create({
    instance: { dbName: "ecommerce" },
  });
  const databaseUrl = mongod.getUri("ecommerce");
  return {
    databaseUrl,
    stop: async () => {
      await mongod.stop();
    },
  };
}

let ferretdbProc: ChildProcessWithoutNullStreams | null = null;

async function waitForPort(port: number, timeoutMs = 20_000): Promise<void> {
  const net = await import("node:net");
  const deadline = Date.now() + timeoutMs;
  while (Date.now() < deadline) {
    try {
      await new Promise<void>((resolve, reject) => {
        const s = net.connect(port, "127.0.0.1", () => {
          s.end();
          resolve();
        });
        s.on("error", reject);
      });
      return;
    } catch {
      await new Promise((r) => setTimeout(r, 250));
    }
  }
  throw new Error(`Nothing listening on 127.0.0.1:${port}`);
}

/** FerretDB v2 + DocumentDB (Postgres). Schema push only unless Prisma probe passes. */
export async function tryFerretDB(): Promise<MongoHandle | null> {
  const ferretBin = process.env.FERRETDB_BIN ?? "/tmp/ferretdb";
  if (!fs.existsSync(ferretBin)) return null;

  try {
    await waitForPort(27017, 800);
    return {
      databaseUrl:
        process.env.FERRETDB_DATABASE_URL ??
        "mongodb://ferret:ferret@127.0.0.1:27017/ecommerce?authSource=admin&directConnection=true",
      stop: async () => undefined,
    };
  } catch {
    /* not running */
  }

  ferretdbProc = spawn(
    ferretBin,
    ["--postgresql-url=postgres://ferret:ferret@127.0.0.1:5432/postgres"],
    { stdio: "ignore" },
  );
  await waitForPort(27017);
  return {
    databaseUrl:
      "mongodb://ferret:ferret@127.0.0.1:27017/ecommerce?authSource=admin&directConnection=true",
    stop: async () => {
      ferretdbProc?.kill("SIGTERM");
      ferretdbProc = null;
    },
  };
}

/**
 * Resolve a Mongo URL that supports Prisma writes (real mongod + replica set).
 * Order: explicit DATABASE_URL → mongodb-memory-server → error with setup hints.
 */
export async function ensureMongoForPrisma(): Promise<MongoHandle> {
  const fromEnv = process.env.DATABASE_URL;
  if (fromEnv) {
    if (await probePrismaWrites(fromEnv)) {
      console.log("[ensure-mongodb] Using DATABASE_URL from environment");
      return { databaseUrl: fromEnv, stop: async () => undefined };
    }
    console.warn(
      "[ensure-mongodb] DATABASE_URL is set but Prisma writes failed (need replica-set MongoDB, not FerretDB alone).",
    );
  }

  try {
    const mem = await startMemoryServer();
    if (await probePrismaWrites(mem.databaseUrl)) {
      console.log("[ensure-mongodb] Using mongodb-memory-server");
      return mem;
    }
    await mem.stop();
  } catch (err) {
    console.warn("[ensure-mongodb] mongodb-memory-server failed:", (err as Error).message);
  }

  const ferret = await tryFerretDB();
  if (ferret && (await probePrismaWrites(ferret.databaseUrl))) {
    return ferret;
  }
  if (ferret) await ferret.stop();

  console.error(`
[ensure-mongodb] No working MongoDB for Prisma.

Prisma on MongoDB requires a replica-set deployment (transactions). Options:
  1. Cloud Agent / CI: add DATABASE_URL (MongoDB Atlas) to environment secrets.
  2. Allow egress to fastdl.mongodb.org and downloads.mongodb.com, then re-run (mongodb-memory-server).
  3. Local: run ./scripts/setup-documentdb-ferretdb.sh for FerretDB (schema push only — not compatible with Prisma writes).

See web/docs/DATABASE.md
`);
  process.exit(1);
}

export function runPrismaPushSeed(databaseUrl: string): void {
  const env = { ...process.env, DATABASE_URL: databaseUrl };
  const push = spawnSync("npx", ["prisma", "db", "push"], {
    cwd: webRoot,
    env,
    stdio: "inherit",
  });
  if (push.status !== 0) process.exit(push.status ?? 1);

  const seed = spawnSync("npx", ["tsx", "prisma/seed.ts"], {
    cwd: webRoot,
    env,
    stdio: "inherit",
  });
  if (seed.status !== 0) process.exit(seed.status ?? 1);
}
