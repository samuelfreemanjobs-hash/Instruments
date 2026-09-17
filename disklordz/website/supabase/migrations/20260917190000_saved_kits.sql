-- WO-SAAS-002: saved drum kits per authenticated user

create table if not exists public.saved_kits (
  id uuid primary key default gen_random_uuid(),
  user_id uuid not null references auth.users (id) on delete cascade,
  kit_id text not null,
  prompt text not null,
  preset_id text not null,
  artist_lane text not null,
  manifest jsonb not null,
  created_at timestamptz not null default now(),
  unique (user_id, kit_id)
);

create index if not exists saved_kits_user_created_idx
  on public.saved_kits (user_id, created_at desc);

alter table public.saved_kits enable row level security;

create policy "Users read own kits"
  on public.saved_kits
  for select
  to authenticated
  using (auth.uid() = user_id);

create policy "Users insert own kits"
  on public.saved_kits
  for insert
  to authenticated
  with check (auth.uid() = user_id);

create policy "Users update own kits"
  on public.saved_kits
  for update
  to authenticated
  using (auth.uid() = user_id)
  with check (auth.uid() = user_id);

create policy "Users delete own kits"
  on public.saved_kits
  for delete
  to authenticated
  using (auth.uid() = user_id);
