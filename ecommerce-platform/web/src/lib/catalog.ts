import { prisma } from "@/lib/db";

export async function getFeaturedProducts() {
  try {
    return await prisma.product.findMany({
      where: { featured: true },
      orderBy: { updatedAt: "desc" },
      take: 8,
      include: { category: true },
    });
  } catch {
    return [];
  }
}

export async function getAllProducts() {
  try {
    return await prisma.product.findMany({
      orderBy: { createdAt: "desc" },
      include: { category: true },
    });
  } catch {
    return [];
  }
}

export async function getProductBySlug(slug: string) {
  try {
    return await prisma.product.findUnique({
      where: { slug },
      include: { category: true, reviews: { where: { approved: true } } },
    });
  } catch {
    return null;
  }
}

export async function getCategories() {
  try {
    return await prisma.category.findMany({ orderBy: { name: "asc" } });
  } catch {
    return [];
  }
}

export async function getActiveTopBar() {
  try {
    return await prisma.topBar.findFirst({ where: { active: true }, orderBy: { updatedAt: "desc" } });
  } catch {
    return null;
  }
}
