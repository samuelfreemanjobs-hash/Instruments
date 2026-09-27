import {
  getBillingSnapshot,
  refundGenerationCredit,
  spendGenerationCredit,
} from "@/lib/credits";
import type { GenerationSpec } from "@/lib/generation/generation-spec";
import { creditCostForSpec } from "@/lib/generation/mode-utils";
import { buildVariationBatch } from "@/lib/generation/variations";
import { activeKitStorageBackend, ensureKitStorageReady } from "@/lib/kit-storage";
import { saveKitForUser } from "@/lib/kits/persist";
import { getPreset, STYLE_PRESETS } from "@/lib/presets";
import { checkRateLimit } from "@/lib/rate-limit";
import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";
import type { KitManifest } from "@/lib/manifest";

export type RunBatchInput = {
  prompt: string;
  presetId: string;
  spec: GenerationSpec;
  baseUrl: string;
  ip: string;
  batchId: string;
  userId: string | null;
};

export type RunBatchSuccess = {
  ok: true;
  manifests: KitManifest[];
  creditCost: number;
  savedToAccount: boolean;
  rateLimit?: { remaining: number; limit: number };
  billing?: Awaited<ReturnType<typeof getBillingSnapshot>>;
};

export type RunBatchFailure = {
  ok: false;
  status: number;
  body: Record<string, unknown>;
  refundCredits?: { userId: string; batchId: string; creditCost: number };
};

export function validateGenerateInput(
  prompt: string,
  presetId: string,
): { ok: true } | { ok: false; status: number; body: Record<string, unknown> } {
  if (!prompt || prompt.length < 3) {
    return {
      ok: false,
      status: 400,
      body: { error: "prompt_required", message: "Describe your vibe (at least 3 characters)." },
    };
  }
  if (!getPreset(presetId)) {
    return { ok: false, status: 400, body: { error: "invalid_preset" } };
  }
  return { ok: true };
}

export async function prepareGenerationAccess(input: {
  userId: string | null;
  ip: string;
  spec: GenerationSpec;
  batchId: string;
}): Promise<
  | { ok: true; creditCost: number; rateLimit?: { remaining: number; limit: number } }
  | RunBatchFailure
> {
  const { userId, ip, spec, batchId } = input;
  const billingActive = Boolean(userId && isSupabaseConfigured());
  const creditCost = creditCostForSpec(spec);

  if (billingActive && userId) {
    const spent = await spendGenerationCredit(userId, batchId, creditCost);
    if (!spent) {
      const billing = await getBillingSnapshot(userId);
      return {
        ok: false,
        status: 402,
        body: {
          error: "insufficient_credits",
          message: "Not enough credits for this batch. Upgrade to Pro or try a lighter mode.",
          creditsBalance: billing?.creditsBalance ?? 0,
          generationCreditCost: creditCost,
          plan: billing?.plan ?? "free",
        },
      };
    }
  } else {
    const limited = checkRateLimit(ip);
    if (!limited.ok) {
      return {
        ok: false,
        status: 429,
        body: {
          error: "daily_limit",
          message: `Guest tier allows ${limited.limit} batches per day per IP. Sign in for credit wallet.`,
          retryAfterSec: limited.retryAfterSec,
          limit: limited.limit,
        },
      };
    }
    return {
      ok: true,
      creditCost,
      rateLimit: { remaining: limited.remaining, limit: limited.limit },
    };
  }

  return { ok: true, creditCost };
}

export async function buildGenerationManifests(input: {
  prompt: string;
  presetId: string;
  spec: GenerationSpec;
  baseUrl: string;
  batchId: string;
  userId: string | null;
}): Promise<RunBatchSuccess | RunBatchFailure> {
  const { prompt, presetId, spec, baseUrl, batchId, userId } = input;
  const creditCost = creditCostForSpec(spec);
  const billingActive = Boolean(userId && isSupabaseConfigured());

  if (activeKitStorageBackend() === "supabase") {
    try {
      await ensureKitStorageReady();
    } catch (err) {
      const message = err instanceof Error ? err.message : "storage_unavailable";
      return {
        ok: false,
        status: 503,
        body: { error: "storage_setup_failed", message },
      };
    }
  }

  let manifests: KitManifest[];
  try {
    const result = await buildVariationBatch(prompt, presetId, baseUrl, spec, batchId);
    manifests = result.manifests;
  } catch (err) {
    const message = err instanceof Error ? err.message : "generation_failed";
    return {
      ok: false,
      status: 500,
      body: { error: "generation_failed", message },
    };
  }

  let savedToAccount = false;
  if (userId && isSupabaseConfigured()) {
    const supabase = await createClient();
    if (supabase) {
      for (const manifest of manifests) {
        const saved = await saveKitForUser(supabase, userId, manifest);
        if (saved.ok) savedToAccount = true;
      }
    }
  }

  let billing: Awaited<ReturnType<typeof getBillingSnapshot>> | undefined;
  if (billingActive && userId) {
    billing = (await getBillingSnapshot(userId)) ?? undefined;
  }

  return {
    ok: true,
    manifests,
    creditCost,
    savedToAccount,
    billing: billing ?? undefined,
  };
}

export async function runGenerateBatch(input: RunBatchInput): Promise<RunBatchSuccess | RunBatchFailure> {
  const access = await prepareGenerationAccess({
    userId: input.userId,
    ip: input.ip,
    spec: input.spec,
    batchId: input.batchId,
  });
  if (access.ok === false) {
    return access;
  }

  const built = await buildGenerationManifests(input);
  if (!built.ok) {
    if (input.userId && isSupabaseConfigured()) {
      await refundGenerationCredit(input.userId, input.batchId, access.creditCost);
    }
    return built;
  }

  return { ...built, rateLimit: access.rateLimit };
}

export { STYLE_PRESETS };
