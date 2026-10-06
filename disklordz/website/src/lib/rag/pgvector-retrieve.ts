import { getSupabaseAdmin } from "@/lib/supabase/admin";

export type PgKnowledgeHit = {
  id: string;
  source_path: string;
  content: string;
  similarity: number;
};

async function embedQuery(text: string): Promise<number[] | null> {
  const apiKey = process.env.OPENAI_API_KEY;
  if (!apiKey) {
    return null;
  }
  const model = process.env.OPENAI_EMBEDDING_MODEL ?? "text-embedding-3-small";
  const res = await fetch("https://api.openai.com/v1/embeddings", {
    method: "POST",
    headers: {
      Authorization: `Bearer ${apiKey}`,
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ input: text, model }),
  });
  if (!res.ok) {
    return null;
  }
  const json = (await res.json()) as { data?: { embedding: number[] }[] };
  return json.data?.[0]?.embedding ?? null;
}

export async function retrievePgvector(
  query: string,
  limit = 3,
): Promise<PgKnowledgeHit[]> {
  const admin = getSupabaseAdmin();
  if (!admin) {
    return [];
  }
  const embedding = await embedQuery(query);
  if (!embedding) {
    return [];
  }
  const { data, error } = await admin.rpc("match_prompt_knowledge", {
    query_embedding: embedding,
    match_count: limit,
    match_threshold: 0.45,
  });
  if (error || !data) {
    return [];
  }
  return (data as PgKnowledgeHit[]).map((row) => ({
    id: row.id,
    source_path: row.source_path,
    content: row.content,
    similarity: row.similarity,
  }));
}
