import Link from "next/link";
import { redirect } from "next/navigation";

import { AccountBilling } from "@/components/AccountBilling";
import { AccountKits } from "@/components/AccountKits";
import { getBillingSnapshot } from "@/lib/credits";
import { isStripeConfigured } from "@/lib/stripe/server";
import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export default async function AccountPage() {
  if (!isSupabaseConfigured()) {
    return (
      <div className="min-h-screen bg-zinc-950 px-4 py-12 text-zinc-100">
        <p className="mx-auto max-w-lg text-zinc-400">
          Configure Supabase env vars to enable accounts. See{" "}
          <code className="text-zinc-300">.env.example</code>.
        </p>
        <Link href="/" className="mt-4 inline-block text-emerald-400">← Home</Link>
      </div>
    );
  }

  const supabase = await createClient();
  if (!supabase) {
    redirect("/login");
  }

  const {
    data: { user },
  } = await supabase.auth.getUser();
  if (!user) {
    redirect("/login");
  }

  const { data: kits } = await supabase
    .from("saved_kits")
    .select("kit_id, prompt, preset_id, artist_lane, created_at")
    .eq("user_id", user.id)
    .order("created_at", { ascending: false })
    .limit(50);

  const billing = await getBillingSnapshot(user.id);

  return (
    <div className="min-h-screen bg-zinc-950 px-4 py-12 text-zinc-100">
      <div className="mx-auto max-w-3xl space-y-6">
        <div className="flex items-center justify-between">
          <h1 className="text-2xl font-semibold">Your kits</h1>
          <Link href="/" className="text-sm text-emerald-400">New kit</Link>
        </div>
        <p className="text-sm text-zinc-500">{user.email}</p>
        <AccountBilling
          plan={billing?.plan ?? "free"}
          creditsBalance={billing?.creditsBalance ?? 0}
          stripeEnabled={isStripeConfigured()}
        />
        <AccountKits kits={kits ?? []} />
      </div>
    </div>
  );
}
