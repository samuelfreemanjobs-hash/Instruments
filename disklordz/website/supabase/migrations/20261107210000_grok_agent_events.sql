-- Grok Bot → PWA completion webhooks (service role writes only)
create table if not exists public.grok_agent_events (
  id uuid primary key default gen_random_uuid(),
  client_ref text not null,
  status text not null default 'unknown',
  summary text not null,
  created_at timestamptz not null default now(),
  constraint grok_agent_events_summary_len check (char_length(summary) <= 16000)
);

create index if not exists grok_agent_events_client_ref_created
  on public.grok_agent_events (client_ref, created_at desc);

alter table public.grok_agent_events enable row level security;

-- No public policies: server uses service role for insert/select from API routes
