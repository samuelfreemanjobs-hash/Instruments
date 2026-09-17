/**
 * End-to-end quick test (SOP-09/10 flow) without browser.
 * Requires mongodb-memory-server binary download (or set DATABASE_URL in env).
 */
import { spawnSync } from "node:child_process";
import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";
import { MongoMemoryServer } from "mongodb-memory-server";
import { PrismaClient } from "@prisma/client";

const root = path.join(path.dirname(fileURLToPath(import.meta.url)), "..");
const envPath = path.join(root, ".env.local");

async function main() {
  if (fs.existsSync(envPath)) {
    const raw = fs.readFileSync(envPath, "utf8");
    for (const line of raw.split("\n")) {
      const m = line.match(/^DATABASE_URL="(.+)"/);
      if (m) process.env.DATABASE_URL = m[1];
    }
  }

  let databaseUrl = process.env.DATABASE_URL;
  let mongod: MongoMemoryServer | null = null;

  if (!databaseUrl) {
    console.log("Starting in-memory MongoDB…");
    mongod = await MongoMemoryServer.create({ instance: { dbName: "ecommerce" } });
    databaseUrl = mongod.getUri();
  }

  const envLines = [
    `DATABASE_URL="${databaseUrl}"`,
    "SESSION_SECRET=e2e-test-session-secret",
    'NEXT_PUBLIC_APP_URL="http://localhost:3000"',
    "# Cloudinary/SMTP optional — email uses dev console logging",
  ];
  fs.writeFileSync(envPath, envLines.join("\n") + "\n");
  console.log("Wrote .env.local");

  const env = { ...process.env, DATABASE_URL: databaseUrl };
  const push = spawnSync("npx", ["prisma", "db", "push"], { cwd: root, env, stdio: "inherit" });
  if (push.status !== 0) process.exit(1);

  const seed = spawnSync("npx", ["tsx", "prisma/seed.ts"], { cwd: root, env, stdio: "inherit" });
  if (seed.status !== 0) process.exit(1);

  const prisma = new PrismaClient({ datasources: { db: { url: databaseUrl } } });

  const demoImage = "https://res.cloudinary.com/demo/image/upload/w_400,c_fill/sample.jpg";
  const product = await prisma.product.findFirst({ where: { slug: "wireless-headphones" } });
  if (product) {
    await prisma.product.update({
      where: { id: product.id },
      data: { images: [demoImage] },
    });
    console.log("Updated product image (simulates Cloudinary save):", product.title);
  }

  const customer = await prisma.user.findUnique({ where: { email: "customer@example.com" } });
  if (!customer) throw new Error("Seed customer missing");

  const order = await prisma.order.create({
    data: {
      userId: customer.id,
      status: "PENDING",
      subtotal: 79.99,
      shipping: 5.99,
      discount: 0,
      total: 85.98,
      shippingName: "Demo Customer",
      shippingLine1: "123 Test St",
      shippingCity: "Austin",
      shippingZip: "78701",
      shippingCountry: "US",
      items: {
        create: [
          {
            productId: product!.id,
            title: product!.title,
            price: product!.price,
            quantity: 1,
          },
        ],
      },
    },
  });
  console.log("Created PENDING order:", order.id);

  const { fulfillOrderPayment } = await import("../src/lib/payments/fulfill.ts");
  const result = await fulfillOrderPayment({
    orderId: order.id,
    provider: "STRIPE",
    paymentRef: `e2e_test_${Date.now()}`,
  });
  console.log("Fulfill result:", result);

  const paid = await prisma.order.findUnique({ where: { id: order.id } });
  console.log("Order status after pay:", paid?.status);

  await prisma.$disconnect();
  if (mongod) await mongod.stop();
  console.log("\nE2E script done. Restart `npm run dev` to pick up .env.local for UI verification.");
}

main().catch((e) => {
  console.error(e);
  process.exit(1);
});
