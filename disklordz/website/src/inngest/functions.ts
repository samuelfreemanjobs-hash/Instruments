import { inngest } from "@/inngest/client";
import { buildVariationBatch } from "@/lib/generation/variations";
import type { GenerationSpec } from "@/lib/generation/generation-spec";

export const generateKitAsync = inngest.createFunction(
  { id: "disklordz-generate-kit-async", retries: 2 },
  { event: "disklordz/generate.requested" },
  async ({ event, step }) => {
    const {
      prompt,
      presetId,
      spec,
      baseUrl,
      userId,
      batchId,
    } = event.data as {
      prompt: string;
      presetId: string;
      spec: GenerationSpec;
      baseUrl: string;
      userId?: string | null;
      batchId: string;
    };

    const result = await step.run("build-variation-batch", async () =>
      buildVariationBatch(prompt, presetId, baseUrl, spec, batchId),
    );

    return {
      batchId: result.batchId,
      kits: result.manifests.length,
      userId: userId ?? null,
    };
  },
);

export const ragReindexRequested = inngest.createFunction(
  { id: "disklordz-rag-reindex", retries: 1 },
  { event: "disklordz/rag.reindex" },
  async ({ step }) => {
    await step.run("note", async () => ({
      message:
        "Run disklordz/rag/scripts/chunk_corpus.py && embed_and_upsert.py in CI or locally.",
    }));
    return { ok: true };
  },
);

export const inngestFunctions = [generateKitAsync, ragReindexRequested];
