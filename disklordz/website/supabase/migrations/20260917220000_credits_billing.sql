-- WO-SAAS-010: credits ledger + Pro plan (Stripe webhook updates plan)

create table if not exists public.user_billing (
  user_id uuid primary key references auth.users (id) on delete cascade,
  plan text not null default 'free' check (plan in ('free', 'pro')),
  credits_balance integer not null default 50 check (credits_balance >= 0),
  stripe_customer_id text,
  stripe_subscription_id text,
  created_at timestamptz not null default now(),
  updated_at timestamptz not null default now()
);

create table if not exists public.credit_ledger (
  id uuid primary key default gen_random_uuid(),
  user_id uuid not null references auth.users (id) on delete cascade,
  delta integer not null,
  reason text not null,
  batch_id text,
  idempotency_key text unique,
  created_at timestamptz not null default now()
);

create index if not exists credit_ledger_user_created_idx
  on public.credit_ledger (user_id, created_at desc);

alter table public.user_billing enable row level security;
alter table public.credit_ledger enable row level security;

create policy "Users read own billing"
  on public.user_billing for select to authenticated
  using (auth.uid() = user_id);

create policy "Users read own ledger"
  on public.credit_ledger for select to authenticated
  using (auth.uid() = user_id);

-- Inserts/updates via security definer functions + service role (Stripe webhook)

create or replace function public.ensure_user_billing(p_user_id uuid)
returns void
language plpgsql
security definer
set search_path = public
as $$
begin
  insert into public.user_billing (user_id)
  values (p_user_id)
  on conflict (user_id) do nothing;
end;
$$;

create or replace function public.spend_generation_credit(p_user_id uuid, p_batch_id text)
returns boolean
language plpgsql
security definer
set search_path = public
as $$
declare
  bal integer;
  uplan text;
begin
  perform public.ensure_user_billing(p_user_id);

  select credits_balance, plan into bal, uplan
  from public.user_billing
  where user_id = p_user_id
  for update;

  if uplan = 'pro' then
    insert into public.credit_ledger (user_id, delta, reason, batch_id, idempotency_key)
    values (p_user_id, 0, 'pro_unlimited', p_batch_id, 'spend:' || p_batch_id)
    on conflict (idempotency_key) do nothing;
    return true;
  end if;

  if bal < 1 then
    return false;
  end if;

  update public.user_billing
  set credits_balance = credits_balance - 1, updated_at = now()
  where user_id = p_user_id;

  insert into public.credit_ledger (user_id, delta, reason, batch_id, idempotency_key)
  values (p_user_id, -1, 'generation_commit', p_batch_id, 'spend:' || p_batch_id);

  return true;
end;
$$;

create or replace function public.refund_generation_credit(p_user_id uuid, p_batch_id text)
returns void
language plpgsql
security definer
set search_path = public
as $$
declare
  uplan text;
begin
  select plan into uplan from public.user_billing where user_id = p_user_id;
  if uplan = 'pro' then
    return;
  end if;

  update public.user_billing
  set credits_balance = credits_balance + 1, updated_at = now()
  where user_id = p_user_id;

  insert into public.credit_ledger (user_id, delta, reason, batch_id, idempotency_key)
  values (p_user_id, 1, 'generation_refund', p_batch_id, 'refund:' || p_batch_id)
  on conflict (idempotency_key) do nothing;
end;
$$;

grant execute on function public.ensure_user_billing(uuid) to service_role;
grant execute on function public.spend_generation_credit(uuid, text) to service_role;
grant execute on function public.refund_generation_credit(uuid, text) to service_role;
