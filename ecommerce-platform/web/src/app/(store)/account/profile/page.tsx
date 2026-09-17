import { redirect } from "next/navigation";
import { getSession } from "@/lib/auth/session";
import { prisma } from "@/lib/db";
import { ProfileForm } from "@/components/store/ProfileForm";

export const dynamic = "force-dynamic";

export default async function ProfilePage() {
  const session = await getSession();
  if (!session) redirect("/auth/login?next=/account/profile");

  let user = null;
  try {
    user = await prisma.user.findUnique({ where: { id: session.userId } });
  } catch {
    user = null;
  }
  if (!user) redirect("/auth/login");

  return (
    <div className="mx-auto max-w-md px-4 py-16">
      <h1 className="text-2xl font-semibold">Your profile</h1>
      <p className="mt-2 text-sm text-slate-600">Role: {user.role}</p>
      <ProfileForm defaultName={user.name ?? ""} email={user.email} />
    </div>
  );
}
