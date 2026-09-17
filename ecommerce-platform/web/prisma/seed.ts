import { PrismaClient, Role, type User, type Category } from "@prisma/client";
import bcrypt from "bcryptjs";
const prisma = new PrismaClient();

/** findUnique + create — FerretDB does not implement upsert $and yet. */
async function ensureUser(data: {
  email: string;
  name: string;
  passwordHash: string;
  role: Role;
}): Promise<User> {
  const existing = await prisma.user.findFirst({ where: { email: data.email } });
  if (existing) return existing;
  return prisma.user.create({ data });
}

async function ensureCategory(data: { name: string; slug: string }): Promise<Category> {
  const existing = await prisma.category.findFirst({ where: { slug: data.slug } });
  if (existing) return existing;
  return prisma.category.create({ data });
}

async function main() {
  const passwordHash = await bcrypt.hash("admin123", 12);
  await ensureUser({
    email: "admin@example.com",
    name: "Shop Admin",
    passwordHash,
    role: Role.ADMIN,
  });

  const customerHash = await bcrypt.hash("customer123", 12);
  await ensureUser({
    email: "customer@example.com",
    name: "Demo Customer",
    passwordHash: customerHash,
    role: Role.CUSTOMER,
  });

  const electronics = await ensureCategory({ name: "Electronics", slug: "electronics" });
  await ensureCategory({ name: "Accessories", slug: "accessories" });

  const products = [
    {
      title: "Wireless Headphones",
      slug: "wireless-headphones",
      description: "Noise-cancelling over-ear headphones with 30h battery life.",
      price: 79.99,
      stock: 42,
      featured: true,
    },
    {
      title: "USB-C Hub",
      slug: "usb-c-hub",
      description: "7-in-1 hub with HDMI, USB 3.0, and SD card reader.",
      price: 49.0,
      stock: 18,
      featured: true,
    },
  ];

  for (const p of products) {
    const existing = await prisma.product.findFirst({ where: { slug: p.slug } });
    if (existing) {
      await prisma.product.update({
        where: { id: existing.id },
        data: { ...p, categoryId: electronics.id },
      });
    } else {
      await prisma.product.create({
        data: { ...p, categoryId: electronics.id, images: [] },
      });
    }
  }

  const coupon = await prisma.coupon.findFirst({ where: { code: "WELCOME10" } });
  if (!coupon) {
    await prisma.coupon.create({
      data: {
        code: "WELCOME10",
        discountPct: 10,
        minCart: 25,
        active: true,
      },
    });
  }

  const topBar = await prisma.topBar.findFirst({ where: { active: true } });
  if (!topBar) {
    await prisma.topBar.create({
      data: {
        message: "Free shipping on orders over $50",
        link: "/shop",
        active: true,
      },
    });
  }

  console.log(
    "Seed complete: admin@example.com / admin123, customer@example.com / customer123, catalog, coupon",
  );
}

main()
  .then(() => prisma.$disconnect())
  .catch(async (e) => {
    console.error(e);
    await prisma.$disconnect();
    process.exit(1);
  });
