import type { SupabaseClient } from "@supabase/supabase-js";

import { emailForStep, type SequenceStep } from "@/lib/launch/email-sequence";

async function sendResend(to: string, subject: string, html: string): Promise<void> {
  const apiKey = process.env.RESEND_API_KEY;
  const from = process.env.RESEND_FROM_EMAIL;
  if (!apiKey || !from) {
    throw new Error("Resend not configured");
  }
  const res = await fetch("https://api.resend.com/emails", {
    method: "POST",
    headers: {
      Authorization: `Bearer ${apiKey}`,
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ from, to: [to], subject, html }),
  });
  if (!res.ok) {
    throw new Error(`Resend ${res.status}`);
  }
}

export async function processDueLaunchEmails(
  supabase: SupabaseClient,
  limit = 50,
): Promise<{ sent: number; failed: number }> {
  const now = new Date().toISOString();
  const { data: due, error } = await supabase
    .from("launch_scheduled_emails")
    .select("id, sequence_step, lead_id")
    .eq("status", "pending")
    .lte("send_at", now)
    .limit(limit);

  if (error || !due) {
    throw new Error(error?.message ?? "queue fetch failed");
  }

  let sent = 0;
  let failed = 0;

  for (const row of due) {
    const { data: leadRow } = await supabase
      .from("launch_leads")
      .select("email")
      .eq("id", row.lead_id)
      .maybeSingle();
    const email = leadRow?.email;
    if (!email) {
      failed += 1;
      continue;
    }
    const step = row.sequence_step as SequenceStep;
    const { subject, html } = emailForStep(step);
    try {
      await sendResend(email, subject, html);
      await supabase
        .from("launch_scheduled_emails")
        .update({ status: "sent", sent_at: new Date().toISOString(), last_error: null })
        .eq("id", row.id);
      sent += 1;
    } catch (e) {
      failed += 1;
      await supabase
        .from("launch_scheduled_emails")
        .update({
          status: "failed",
          last_error: e instanceof Error ? e.message : "send failed",
        })
        .eq("id", row.id);
    }
  }

  return { sent, failed };
}
