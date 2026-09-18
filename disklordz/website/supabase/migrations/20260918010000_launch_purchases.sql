-- Launch one-time purchases (tripwire sample, etc.)

create table if not exists public.launch_purchases (
  id uuid primary key default gen_random_uuid(),
  sku text not null,
  email text,
  stripe_session_id text not null unique,
  amount_total bigint,
  currency text,
  created_at timestamptz not null default now()
);

create index if not exists launch_purchases_sku_idx on public.launch_purchases (sku);

alter table public.launch_purchases enable row level security;
