import { Role } from "@prisma/client";
import { redirect } from "next/navigation";
import { AdminShell } from "@/components/admin/AdminShell";
import { getSession } from "@/lib/auth/session";

export const dynamic = "force-dynamic";

export default async function AdminLayout({ children }: { children: React.ReactNode }) {
  const session = await getSession();
  if (!session) redirect("/auth/login?next=/admin");
  if (session.role !== Role.ADMIN) redirect("/?error=admin-required");

  return <AdminShell>{children}</AdminShell>;
}
