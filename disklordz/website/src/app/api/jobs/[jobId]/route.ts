import { NextRequest, NextResponse } from "next/server";

import { executeGenerationJob, getGenerationJob } from "@/lib/jobs/generation-jobs";
import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export async function GET(
  req: NextRequest,
  ctx: { params: Promise<{ jobId: string }> },
) {
  const { jobId } = await ctx.params;
  const job = await getGenerationJob(jobId);
  if (!job) {
    return NextResponse.json({ error: "not_found" }, { status: 404 });
  }

  if (isSupabaseConfigured()) {
    const supabase = await createClient();
    if (supabase) {
      const {
        data: { user },
      } = await supabase.auth.getUser();
      if (job.user_id && user?.id !== job.user_id) {
        return NextResponse.json({ error: "forbidden" }, { status: 403 });
      }
    }
  }

  if (job.status === "queued") {
    void executeGenerationJob(jobId, req.nextUrl.origin);
  }

  const fresh = (await getGenerationJob(jobId)) ?? job;

  return NextResponse.json({
    jobId: fresh.id,
    status: fresh.status,
    error: fresh.error_code ? { code: fresh.error_code, message: fresh.error_message } : undefined,
    variations: fresh.result?.variations,
    batchId: fresh.batch_id,
  });
}
