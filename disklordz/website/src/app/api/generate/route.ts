import { NextRequest, NextResponse, after } from "next/server";

import { logGenerationEvent } from "@/lib/analytics/generation-events";
import { refundGenerationCredit, newBatchId } from "@/lib/credits";
import { parseGenerationSpec } from "@/lib/generation/generation-spec";
import { creditCostForSpec } from "@/lib/generation/mode-utils";
import {
  buildGenerationManifests,
  prepareGenerationAccess,
  runGenerateBatch,
  STYLE_PRESETS,
  validateGenerateInput,
} from "@/lib/generation/run-batch";
import {
  asyncJobsEnabled,
  createGenerationJob,
  executeGenerationJob,
} from "@/lib/jobs/generation-jobs";
import { activeKitStorageBackend } from "@/lib/kit-storage";
import { isSupabaseConfigured } from "@/lib/supabase/config";
import { createClient } from "@/lib/supabase/server";

function jsonFromBatchResult(
  batchId: string,
  result: Extract<Awaited<ReturnType<typeof runGenerateBatch>>, { ok: true }>,
) {
  const variations = result.manifests.map((manifest) => ({
    label: manifest.variationLabel ?? "A",
    manifest,
  }));

  return {
    batchId,
    creditCost: result.creditCost,
    variationCount: variations.length,
    storageBackend: activeKitStorageBackend(),
    variations,
    manifest: result.manifests[0],
    savedToAccount: result.savedToAccount,
    rateLimit: result.rateLimit,
    billing: result.billing
      ? {
          plan: result.billing.plan,
          creditsBalance: result.billing.creditsBalance,
          generationCreditCost: result.creditCost,
          unlimited: result.billing.plan === "pro",
        }
      : undefined,
    presets: STYLE_PRESETS.map(({ id, label, description }) => ({
      id,
      label,
      description,
    })),
  };
}

export async function POST(req: NextRequest) {
  const started = Date.now();
  const ip =
    req.headers.get("x-forwarded-for")?.split(",")[0]?.trim() ??
    req.headers.get("x-real-ip") ??
    "anonymous";

  let body: {
    prompt?: string;
    presetId?: string;
    spec?: Record<string, unknown>;
    async?: boolean;
  };
  try {
    body = await req.json();
  } catch {
    return NextResponse.json({ error: "invalid_json" }, { status: 400 });
  }

  const prompt = (body.prompt ?? "").trim();
  const presetId = body.presetId ?? STYLE_PRESETS[0].id;
  const wantAsync = body.async === true;

  const valid = validateGenerateInput(prompt, presetId);
  if (!valid.ok) {
    return NextResponse.json(valid.body, { status: valid.status });
  }

  const parsed = parseGenerationSpec(body.spec, presetId);
  if (!parsed.ok) {
    return NextResponse.json(
      { error: parsed.error, message: "Invalid generation spec." },
      { status: 400 },
    );
  }

  let userId: string | null = null;
  if (isSupabaseConfigured()) {
    const supabase = await createClient();
    if (supabase) {
      const {
        data: { user },
      } = await supabase.auth.getUser();
      userId = user?.id ?? null;
    }
  }

  logGenerationEvent({
    event: "generate_start",
    userId,
    presetId,
    spec: parsed.spec,
    ip,
  });

  const batchId = newBatchId();
  const baseUrl = req.nextUrl.origin;
  const creditCost = creditCostForSpec(parsed.spec);
  const useAsync =
    wantAsync &&
    asyncJobsEnabled() &&
    (parsed.spec.mode === "loop" || parsed.spec.mode === "sfx");

  if (useAsync) {
    const access = await prepareGenerationAccess({
      userId,
      ip,
      spec: parsed.spec,
      batchId,
    });
    if (access.ok === false) {
      logGenerationEvent({
        event: "generate_fail",
        userId,
        presetId,
        spec: parsed.spec,
        ok: false,
        errorCode: String(access.body.error),
        durationMs: Date.now() - started,
        ip,
      });
      return NextResponse.json(access.body, { status: access.status });
    }

    const jobId = await createGenerationJob({
      userId,
      prompt,
      presetId,
      spec: parsed.spec,
      batchId,
      creditCost: access.creditCost,
    });

    if (!jobId) {
      if (userId) {
        await refundGenerationCredit(userId, batchId, access.creditCost);
      }
      return NextResponse.json({ error: "async_job_create_failed" }, { status: 503 });
    }

    after(async () => {
      await executeGenerationJob(jobId, baseUrl);
    });

    return NextResponse.json(
      {
        async: true,
        jobId,
        pollUrl: `/api/jobs/${jobId}`,
        batchId,
        creditCost: access.creditCost,
        rateLimit: access.rateLimit,
      },
      { status: 202 },
    );
  }

  const result = await runGenerateBatch({
    prompt,
    presetId,
    spec: parsed.spec,
    baseUrl,
    ip,
    batchId,
    userId,
  });

  if (result.ok === false) {
    logGenerationEvent({
      event: "generate_fail",
      userId,
      presetId,
      spec: parsed.spec,
      ok: false,
      errorCode: String(result.body.error),
      durationMs: Date.now() - started,
      ip,
    });
    return NextResponse.json(result.body, { status: result.status });
  }

  logGenerationEvent({
    event: "generate_success",
    userId,
    presetId,
    spec: parsed.spec,
    ok: true,
    durationMs: Date.now() - started,
    ip,
    meta: { variationCount: result.manifests.length },
  });

  return NextResponse.json(jsonFromBatchResult(batchId, result));
}
