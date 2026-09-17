"use server";

import { revalidatePath } from "next/cache";
import { z } from "zod";
import { prisma } from "@/lib/db";
import { createSessionToken, getSession, setSessionCookie } from "@/lib/auth/session";

const schema = z.object({ name: z.string().min(2) });

export type ProfileResult = { ok: true } | { ok: false; error: string };

export async function updateProfileAction(input: unknown): Promise<ProfileResult> {
  const session = await getSession();
  if (!session) return { ok: false, error: "Not signed in" };

  const parsed = schema.safeParse(input);
  if (!parsed.success) {
    return { ok: false, error: parsed.error.issues[0]?.message ?? "Invalid input" };
  }

  try {
    const user = await prisma.user.update({
      where: { id: session.userId },
      data: { name: parsed.data.name },
    });
    const token = await createSessionToken({
      userId: user.id,
      role: user.role,
      email: user.email,
      name: user.name,
    });
    await setSessionCookie(token);
    revalidatePath("/account/profile");
    return { ok: true };
  } catch {
    return { ok: false, error: "Update failed" };
  }
}
