import { PrismaClient, Role } from "@prisma/client";
import bcrypt from "bcryptjs";
const prisma = new PrismaClient();

async function main() {
  const passwordHash = await bcrypt.hash("admin123", 12);
  await prisma.user.upsert({
    where: { email: "admin@example.com" },
    update: {},
    create: {
      email: "admin@example.com",
      name: "Shop Admin",
      passwordHash,
      role: Role.ADMIN,
    },
  });

  const electronics = await prisma.category.upsert({
    where: { slug: "electronics" },
    update: {},
    create: { name: "Electronics", slug: "electronics" },
  });

  await prisma.category.upsert({
    where: { slug: "accessories" },
    update: {},
    create: { name: "Accessories", slug: "accessories" },
  });

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
    await prisma.product.upsert({
      where: { slug: p.slug },
      update: { ...p, categoryId: electronics.id },
      create: { ...p, categoryId: electronics.id, images: [] },
    });
  }

  await prisma.coupon.upsert({
    where: { code: "WELCOME10" },
    update: {},
    create: {
      code: "WELCOME10",
      discountPct: 10,
      minCart: 25,
      active: true,
    },
  });

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

  console.log("Seed complete: admin@example.com / admin123, categories, products, coupon");
}

main()
  .then(() => prisma.$disconnect())
  .catch(async (e) => {
    console.error(e);
    await prisma.$disconnect();
    process.exit(1);
  });
