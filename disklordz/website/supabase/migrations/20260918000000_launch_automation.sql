-- Launch funnel automation (FORGE / PM scheduling)
-- Requires service role from API routes + cron

create table if not exists public.launch_leads (
  id uuid primary key default gen_random_uuid(),
  email text not null,
  campaign_id text not null default 'launch-v1',
  opted_in_at timestamptz not null default now(),
  metadata jsonb not null default '{}'::jsonb
);

create unique index if not exists launch_leads_email_campaign_idx
  on public.launch_leads (email, campaign_id);

create table if not exists public.launch_scheduled_emails (
  id uuid primary key default gen_random_uuid(),
  lead_id uuid not null references public.launch_leads (id) on delete cascade,
  sequence_step smallint not null check (sequence_step between 1 and 4),
  send_at timestamptz not null,
  status text not null default 'pending'
    check (status in ('pending', 'sent', 'failed', 'cancelled')),
  last_error text,
  sent_at timestamptz,
  created_at timestamptz not null default now()
);

create index if not exists launch_scheduled_emails_due_idx
  on public.launch_scheduled_emails (status, send_at)
  where status = 'pending';

create table if not exists public.workflow_registry (
  id uuid primary key default gen_random_uuid(),
  agent_id text not null,
  source_path text not null,
  workflow_kind text not null default 'agent_created',
  schedule_cron text,
  config jsonb not null default '{}'::jsonb,
  created_at timestamptz not null default now()
);

create index if not exists workflow_registry_agent_idx on public.workflow_registry (agent_id);

alter table public.launch_leads enable row level security;
alter table public.launch_scheduled_emails enable row level security;
alter table public.workflow_registry enable row level security;

-- No public policies: API uses service role only
