import { redirect } from "next/navigation";

import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";
import { isAdminEmail } from "@/lib/admin";

export default async function AdminLayout({ children }: { children: React.ReactNode }) {
  if (!isSupabaseConfigured()) {
    redirect("/settings");
  }
  const supabase = await createClient();
  const { data } = await supabase!.auth.getUser();
  if (!data.user || !isAdminEmail(data.user.email)) {
    redirect("/login?next=/admin");
  }
  return (
    <div className="space-y-4">
      <p className="text-xs text-bfm-warning">Admin — retailer integrations &amp; catalog ops</p>
      {children}
    </div>
  );
}
