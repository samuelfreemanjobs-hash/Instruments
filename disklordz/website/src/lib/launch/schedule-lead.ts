import type { SupabaseClient } from "@supabase/supabase-js";

import { SEQUENCE_DELAYS_HOURS, type SequenceStep } from "@/lib/launch/email-sequence";

export async function recordLeadAndSchedule(
  supabase: SupabaseClient,
  email: string,
  campaignId = "launch-v1",
): Promise<{ leadId: string } | { error: string }> {
  const normalized = email.toLowerCase();
  const { data: existing } = await supabase
    .from("launch_leads")
    .select("id")
    .eq("email", normalized)
    .eq("campaign_id", campaignId)
    .maybeSingle();

  if (existing) {
    return { leadId: existing.id };
  }

  const { data: lead, error: leadErr } = await supabase
    .from("launch_leads")
    .insert({
      email: normalized,
      campaign_id: campaignId,
      opted_in_at: new Date().toISOString(),
    })
    .select("id")
    .single();

  if (leadErr || !lead) {
    return { error: leadErr?.message ?? "lead insert failed" };
  }

  const optedAt = new Date();
  const rows = ([1, 2, 3, 4] as SequenceStep[]).map((step) => ({
    lead_id: lead.id,
    sequence_step: step,
    send_at: new Date(optedAt.getTime() + SEQUENCE_DELAYS_HOURS[step] * 3600_000).toISOString(),
    status: "pending",
  }));

  const { error: schedErr } = await supabase.from("launch_scheduled_emails").insert(rows);
  if (schedErr) {
    return { error: schedErr.message };
  }

  return { leadId: lead.id };
}
