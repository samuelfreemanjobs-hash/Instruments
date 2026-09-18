import { NextRequest, NextResponse } from "next/server";

import {
  getBillingSnapshot,
  newBatchId,
  refundGenerationCredit,
  spendGenerationCredit,
} from "@/lib/credits";
import { parseGenerationSpec } from "@/lib/generation/generation-spec";
import { PRODUCT_PACK_CREDIT_COST } from "@/lib/generation/mode-utils";
import { agentPipelineAfterFactory } from "@/lib/factory/agent-pipeline";
import { buildProductPack } from "@/lib/generation/product-factory";
import { activeKitStorageBackend, ensureKitStorageReady } from "@/lib/kit-storage";
import { getPreset, STYLE_PRESETS } from "@/lib/presets";
import { checkRateLimit } from "@/lib/rate-limit";
import { getSupabaseAdmin } from "@/lib/supabase/admin";
import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";

export async function POST(req: NextRequest) {
  const ip =
    req.headers.get("x-forwarded-for")?.split(",")[0]?.trim() ??
    req.headers.get("x-real-ip") ??
    "anonymous";

  let body: { prompt?: string; presetId?: string; spec?: Record<string, unknown> };
  try {
    body = await req.json();
  } catch {
    return NextResponse.json({ error: "invalid_json" }, { status: 400 });
  }

  const prompt = (body.prompt ?? "").trim();
  const presetId = body.presetId ?? STYLE_PRESETS[0].id;

  if (!prompt || prompt.length < 3) {
    return NextResponse.json(
      { error: "prompt_required", message: "Describe the pack brief (at least 3 characters)." },
      { status: 400 },
    );
  }

  if (!getPreset(presetId)) {
    return NextResponse.json({ error: "invalid_preset" }, { status: 400 });
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

  const billingActive = Boolean(userId && getSupabaseAdmin());
  const batchId = newBatchId();
  const creditCost = PRODUCT_PACK_CREDIT_COST;

  if (billingActive && userId) {
    const spent = await spendGenerationCredit(userId, batchId, creditCost);
    if (!spent) {
      const billing = await getBillingSnapshot(userId);
      return NextResponse.json(
        {
          error: "insufficient_credits",
          message: `Product packs cost ${creditCost} credits.`,
          creditsBalance: billing?.creditsBalance ?? 0,
          generationCreditCost: creditCost,
          plan: billing?.plan ?? "free",
        },
        { status: 402 },
      );
    }
  } else {
    const limited = checkRateLimit(ip);
    if (!limited.ok) {
      return NextResponse.json(
        {
          error: "daily_limit",
          message: `Guest tier allows ${limited.limit} batches per day per IP. Sign in for product factory.`,
          retryAfterSec: limited.retryAfterSec,
          limit: limited.limit,
        },
        { status: 429 },
      );
    }
  }

  if (activeKitStorageBackend() === "supabase") {
    try {
      await ensureKitStorageReady();
    } catch (err) {
      if (billingActive && userId) {
        await refundGenerationCredit(userId, batchId, creditCost);
      }
      const message = err instanceof Error ? err.message : "storage_unavailable";
      return NextResponse.json(
        { error: "storage_setup_failed", message },
        { status: 503 },
      );
    }
  }

  const baseUrl = req.nextUrl.origin;
  let pack;
  try {
    pack = await buildProductPack(prompt, presetId, baseUrl, parsed.spec, batchId);
  } catch (err) {
    if (billingActive && userId) {
      await refundGenerationCredit(userId, batchId, creditCost);
    }
    const message = err instanceof Error ? err.message : "factory_failed";
    return NextResponse.json({ error: "factory_failed", message }, { status: 500 });
  }

  let creditsAfter: Awaited<ReturnType<typeof getBillingSnapshot>> | null = null;
  if (billingActive && userId) {
    creditsAfter = await getBillingSnapshot(userId);
  }

  return NextResponse.json({
    batchId,
    creditCost,
    storageBackend: activeKitStorageBackend(),
    agentPipeline: agentPipelineAfterFactory(presetId),
    productPack: pack,
    billing: creditsAfter
      ? {
          plan: creditsAfter.plan,
          creditsBalance: creditsAfter.creditsBalance,
          unlimited: creditsAfter.plan === "pro",
        }
      : undefined,
  });
}
