-- WO-SAAS-012b: prompt knowledge chunks for hybrid RAG (pgvector + keyword fallback)
create extension if not exists vector;

create table if not exists public.prompt_knowledge_chunks (
  id text primary key,
  source_path text not null,
  lane_id text,
  content text not null,
  embedding vector(1536),
  metadata jsonb not null default '{}'::jsonb,
  created_at timestamptz not null default now()
);

create index if not exists prompt_knowledge_chunks_embedding_idx
  on public.prompt_knowledge_chunks
  using hnsw (embedding vector_cosine_ops);

alter table public.prompt_knowledge_chunks enable row level security;

create policy "prompt_knowledge_read_authenticated"
  on public.prompt_knowledge_chunks
  for select
  to authenticated
  using (true);

create policy "prompt_knowledge_service_role_all"
  on public.prompt_knowledge_chunks
  for all
  to service_role
  using (true)
  with check (true);

create or replace function public.match_prompt_knowledge(
  query_embedding vector(1536),
  match_count int default 5,
  match_threshold float default 0.5
)
returns table (
  id text,
  source_path text,
  content text,
  similarity float
)
language sql
stable
as $$
  select
    c.id,
    c.source_path,
    c.content,
    1 - (c.embedding <=> query_embedding) as similarity
  from public.prompt_knowledge_chunks c
  where c.embedding is not null
    and 1 - (c.embedding <=> query_embedding) >= match_threshold
  order by c.embedding <=> query_embedding
  limit match_count;
$$;
