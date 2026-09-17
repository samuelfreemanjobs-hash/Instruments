import { NextResponse } from "next/server";
import { prisma } from "@/lib/db";

export async function GET(request: Request) {
  const { searchParams } = new URL(request.url);
  const q = searchParams.get("q")?.trim() ?? "";

  if (q.length < 2) {
    return NextResponse.json({ products: [] });
  }

  try {
    const products = await prisma.product.findMany({
      where: {
        OR: [
          { title: { contains: q, mode: "insensitive" } },
          { description: { contains: q, mode: "insensitive" } },
        ],
      },
      take: 12,
      select: { id: true, title: true, slug: true, price: true, images: true },
    });
    return NextResponse.json({ products });
  } catch {
    return NextResponse.json({ products: [], error: "Database unavailable" }, { status: 503 });
  }
}
