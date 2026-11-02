import type { KnowledgeSnippet } from "@/lib/rag/knowledge-snippets";
import { retrievePgvector } from "@/lib/rag/pgvector-retrieve";
import { retrieveSnippets } from "@/lib/rag/retrieve";

export type HybridRetrieval = {
  snippets: KnowledgeSnippet[];
  retrieval: "keyword_v1" | "pgvector_v1" | "hybrid_v1";
};

/** Prefer pgvector when configured; always merge keyword hits for lane vocabulary. */
export async function retrieveHybrid(query: string, limit = 3): Promise<HybridRetrieval> {
  const keyword = retrieveSnippets(query, limit);
  const pg = await retrievePgvector(query, limit);

  if (!pg.length) {
    return { snippets: keyword, retrieval: "keyword_v1" };
  }

  const fromPg: KnowledgeSnippet[] = pg.map((row) => ({
    id: `pg:${row.id}`,
    text: row.content,
    tags: [row.source_path],
  }));

  const seen = new Set<string>();
  const merged: KnowledgeSnippet[] = [];
  for (const s of [...fromPg, ...keyword]) {
    const key = s.text.slice(0, 80);
    if (seen.has(key)) {
      continue;
    }
    seen.add(key);
    merged.push(s);
    if (merged.length >= limit) {
      break;
    }
  }

  return {
    snippets: merged,
    retrieval: keyword.length ? "hybrid_v1" : "pgvector_v1",
  };
}
