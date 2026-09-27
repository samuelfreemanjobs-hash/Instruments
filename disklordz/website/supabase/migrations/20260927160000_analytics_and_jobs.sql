-- WO-SAAS-025 analytics events + WO-SAAS-026 async generation jobs

create table if not exists public.generation_events (
  id uuid primary key default gen_random_uuid(),
  event text not null,
  user_id uuid references auth.users (id) on delete set null,
  preset_id text,
  mode text,
  engine text,
  ok boolean not null default true,
  error_code text,
  duration_ms integer,
  ip_hash text,
  meta jsonb,
  created_at timestamptz not null default now()
);

create index if not exists generation_events_created_idx
  on public.generation_events (created_at desc);

create index if not exists generation_events_event_created_idx
  on public.generation_events (event, created_at desc);

alter table public.generation_events enable row level security;

-- Ops reads via service role only (no public policies)

create table if not exists public.generation_jobs (
  id uuid primary key default gen_random_uuid(),
  user_id uuid references auth.users (id) on delete set null,
  status text not null default 'queued'
    check (status in ('queued', 'running', 'completed', 'failed')),
  prompt text not null,
  preset_id text not null,
  spec jsonb not null,
  batch_id text,
  credit_cost integer not null default 1,
  result jsonb,
  error_code text,
  error_message text,
  created_at timestamptz not null default now(),
  updated_at timestamptz not null default now()
);

create index if not exists generation_jobs_user_created_idx
  on public.generation_jobs (user_id, created_at desc);

alter table public.generation_jobs enable row level security;

create policy "Users read own generation jobs"
  on public.generation_jobs for select to authenticated
  using (auth.uid() = user_id);
