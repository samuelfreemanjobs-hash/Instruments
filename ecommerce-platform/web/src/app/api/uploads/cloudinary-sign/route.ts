import { NextResponse } from "next/server";
import { Role } from "@prisma/client";
import { getSession } from "@/lib/auth/session";
import { isCloudinaryConfigured, signUploadParams } from "@/lib/cloudinary/server";

export const runtime = "nodejs";

export async function POST(request: Request) {
  const session = await getSession();
  if (!session || session.role !== Role.ADMIN) {
    return NextResponse.json({ error: "Forbidden" }, { status: 403 });
  }

  if (!isCloudinaryConfigured()) {
    return NextResponse.json({ error: "Cloudinary not configured" }, { status: 503 });
  }

  const body = (await request.json().catch(() => ({}))) as { folder?: string };
  const folder = body.folder?.trim() || "ecommerce";

  try {
    const signed = signUploadParams(folder);
    return NextResponse.json(signed);
  } catch {
    return NextResponse.json({ error: "Could not sign upload" }, { status: 500 });
  }
}
