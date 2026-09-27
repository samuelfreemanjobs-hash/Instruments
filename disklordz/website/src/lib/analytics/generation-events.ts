import { createHash } from "crypto";

import type { GenerationSpec } from "@/lib/generation/generation-spec";
import { getSupabaseAdmin } from "@/lib/supabase/admin";

export type GenerationEventName =
  | "generate_start"
  | "generate_success"
  | "generate_fail"
  | "job_complete"
  | "job_fail";

function hashIp(ip: string): string {
  return createHash("sha256").update(`disklordz-ip-v1:${ip}`).digest("hex").slice(0, 16);
}

/** Fire-and-forget analytics (WO-SAAS-025). No PII in meta. */
export function logGenerationEvent(input: {
  event: GenerationEventName;
  userId?: string | null;
  presetId?: string;
  spec?: GenerationSpec;
  ok?: boolean;
  errorCode?: string;
  durationMs?: number;
  ip?: string;
  meta?: Record<string, unknown>;
}): void {
  const admin = getSupabaseAdmin();
  if (!admin) return;

  const row = {
    event: input.event,
    user_id: input.userId ?? null,
    preset_id: input.presetId ?? null,
    mode: input.spec?.mode ?? null,
    engine: input.spec?.engine ?? null,
    ok: input.ok ?? true,
    error_code: input.errorCode ?? null,
    duration_ms: input.durationMs ?? null,
    ip_hash: input.ip ? hashIp(input.ip) : null,
    meta: input.meta ?? null,
  };

  void admin.from("generation_events").insert(row).then(({ error }) => {
    if (error) console.error("analytics_insert_failed");
  });
}
