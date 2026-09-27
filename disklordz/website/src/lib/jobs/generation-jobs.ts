import { buildVariationBatch } from "@/lib/generation/variations";
import type { GenerationSpec } from "@/lib/generation/generation-spec";
import { saveKitForUser } from "@/lib/kits/persist";
import { activeKitStorageBackend, ensureKitStorageReady } from "@/lib/kit-storage";
import type { KitManifest } from "@/lib/manifest";
import { getSupabaseAdmin } from "@/lib/supabase/admin";

export type JobRecord = {
  id: string;
  user_id: string | null;
  status: "queued" | "running" | "completed" | "failed";
  prompt: string;
  preset_id: string;
  spec: GenerationSpec;
  batch_id: string | null;
  credit_cost: number;
  result: { variations: { label: string; manifest: KitManifest }[] } | null;
  error_code: string | null;
  error_message: string | null;
};

export async function createGenerationJob(input: {
  userId: string | null;
  prompt: string;
  presetId: string;
  spec: GenerationSpec;
  batchId: string;
  creditCost: number;
}): Promise<string | null> {
  const admin = getSupabaseAdmin();
  if (!admin) return null;

  const { data, error } = await admin
    .from("generation_jobs")
    .insert({
      user_id: input.userId,
      status: "queued",
      prompt: input.prompt,
      preset_id: input.presetId,
      spec: input.spec,
      batch_id: input.batchId,
      credit_cost: input.creditCost,
    })
    .select("id")
    .single();

  if (error || !data) return null;
  return data.id as string;
}

export async function getGenerationJob(jobId: string): Promise<JobRecord | null> {
  const admin = getSupabaseAdmin();
  if (!admin) return null;

  const { data, error } = await admin.from("generation_jobs").select("*").eq("id", jobId).maybeSingle();
  if (error || !data) return null;
  return data as JobRecord;
}

async function setJobStatus(
  jobId: string,
  patch: Partial<{
    status: string;
    result: unknown;
    error_code: string;
    error_message: string;
  }>,
): Promise<void> {
  const admin = getSupabaseAdmin();
  if (!admin) return;
  await admin
    .from("generation_jobs")
    .update({ ...patch, updated_at: new Date().toISOString() })
    .eq("id", jobId);
}

/** Build kits for a job (credits already spent at enqueue). WO-SAAS-026 */
export async function executeGenerationJob(jobId: string, baseUrl: string): Promise<void> {
  const job = await getGenerationJob(jobId);
  if (!job || job.status === "completed" || job.status === "failed") return;

  await setJobStatus(jobId, { status: "running" });

  try {
    if (activeKitStorageBackend() === "supabase") {
      await ensureKitStorageReady();
    }

    const batchId = job.batch_id ?? jobId;
    const { manifests } = await buildVariationBatch(
      job.prompt,
      job.preset_id,
      baseUrl,
      job.spec,
      batchId,
    );

    const admin = getSupabaseAdmin();
    if (job.user_id && admin) {
      for (const manifest of manifests) {
        await saveKitForUser(admin, job.user_id, manifest);
      }
    }

    const variations = manifests.map((manifest) => ({
      label: manifest.variationLabel ?? "A",
      manifest,
    }));

    await setJobStatus(jobId, { status: "completed", result: { variations } });
  } catch (err) {
    const message = err instanceof Error ? err.message : "generation_failed";
    await setJobStatus(jobId, {
      status: "failed",
      error_code: "generation_failed",
      error_message: message,
    });
  }
}

export function asyncJobsEnabled(): boolean {
  return Boolean(getSupabaseAdmin()) && process.env.DISABLE_ASYNC_JOBS !== "1";
}
