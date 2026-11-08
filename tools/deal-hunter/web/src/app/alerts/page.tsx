import { AlertsClient, type PriceAlertRow } from "./alerts-client";
import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export default async function AlertsPage() {
  let serverAlerts: PriceAlertRow[] = [];
  if (isSupabaseConfigured()) {
    const supabase = await createClient();
    if (supabase) {
      const { data: auth } = await supabase.auth.getUser();
      if (auth.user) {
        const { data } = await supabase
          .from("price_alerts")
          .select("id, external_deal_id, target_price, active, created_at")
          .order("created_at", { ascending: false });
        serverAlerts = (data ?? []) as PriceAlertRow[];
      }
    }
  }

  return (
    <section className="space-y-4">
      <header className="space-y-1">
        <h2 className="text-lg font-semibold">Notifications</h2>
        <p className="text-sm text-bfm-muted">Price alerts in Supabase with RLS; browser prefs stay local until push is wired.</p>
      </header>
      <AlertsClient serverAlerts={serverAlerts} />
    </section>
  );
}
