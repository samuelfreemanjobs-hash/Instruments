"use server";

import { Role } from "@prisma/client";
import bcrypt from "bcryptjs";
import { redirect } from "next/navigation";
import { prisma } from "@/lib/db";
import {
  clearSessionCookie,
  createSessionToken,
  setSessionCookie,
} from "@/lib/auth/session";
import { loginSchema, registerSchema } from "@/lib/validators/auth";

export type AuthResult = { ok: true } | { ok: false; error: string };

export async function loginAction(input: unknown, redirectTo?: string): Promise<AuthResult> {
  const parsed = loginSchema.safeParse(input);
  if (!parsed.success) {
    return { ok: false, error: parsed.error.issues[0]?.message ?? "Invalid input" };
  }

  let user;
  try {
    user = await prisma.user.findUnique({ where: { email: parsed.data.email.toLowerCase() } });
  } catch {
    return { ok: false, error: "Database unavailable" };
  }

  if (!user) {
    return { ok: false, error: "Invalid email or password" };
  }

  const valid = await bcrypt.compare(parsed.data.password, user.passwordHash);
  if (!valid) {
    return { ok: false, error: "Invalid email or password" };
  }

  const token = await createSessionToken({
    userId: user.id,
    role: user.role,
    email: user.email,
    name: user.name,
  });
  await setSessionCookie(token);

  if (redirectTo && redirectTo.startsWith("/")) {
    if (redirectTo.startsWith("/admin") && user.role !== Role.ADMIN) {
      redirect("/?error=admin-required");
    }
    redirect(redirectTo);
  }
  redirect(user.role === Role.ADMIN ? "/admin" : "/account/profile");
}

export async function registerAction(input: unknown): Promise<AuthResult> {
  const parsed = registerSchema.safeParse(input);
  if (!parsed.success) {
    return { ok: false, error: parsed.error.issues[0]?.message ?? "Invalid input" };
  }

  const email = parsed.data.email.toLowerCase();
  const passwordHash = await bcrypt.hash(parsed.data.password, 12);

  try {
    const existing = await prisma.user.findUnique({ where: { email } });
    if (existing) {
      return { ok: false, error: "An account with this email already exists" };
    }

    const user = await prisma.user.create({
      data: {
        email,
        name: parsed.data.name,
        passwordHash,
        role: Role.CUSTOMER,
      },
    });

    const token = await createSessionToken({
      userId: user.id,
      role: user.role,
      email: user.email,
      name: user.name,
    });
    await setSessionCookie(token);
  } catch {
    return { ok: false, error: "Could not create account" };
  }

  redirect("/account/profile");
}

export async function logoutAction() {
  await clearSessionCookie();
  redirect("/");
}
