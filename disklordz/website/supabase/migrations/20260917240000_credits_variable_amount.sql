-- WO-SAAS-014: variable credit spend/refund per generation batch

drop function if exists public.spend_generation_credit(uuid, text);
drop function if exists public.refund_generation_credit(uuid, text);

create or replace function public.spend_generation_credit(
  p_user_id uuid,
  p_batch_id text,
  p_amount integer default 1
)
returns boolean
language plpgsql
security definer
set search_path = public
as $$
declare
  bal integer;
  uplan text;
  amt integer;
begin
  amt := greatest(1, coalesce(p_amount, 1));

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

  if bal < amt then
    return false;
  end if;

  update public.user_billing
  set credits_balance = credits_balance - amt, updated_at = now()
  where user_id = p_user_id;

  insert into public.credit_ledger (user_id, delta, reason, batch_id, idempotency_key)
  values (p_user_id, -amt, 'generation_commit', p_batch_id, 'spend:' || p_batch_id);

  return true;
end;
$$;

create or replace function public.refund_generation_credit(
  p_user_id uuid,
  p_batch_id text,
  p_amount integer default 1
)
returns void
language plpgsql
security definer
set search_path = public
as $$
declare
  uplan text;
  amt integer;
begin
  amt := greatest(1, coalesce(p_amount, 1));

  select plan into uplan from public.user_billing where user_id = p_user_id;
  if uplan = 'pro' then
    return;
  end if;

  update public.user_billing
  set credits_balance = credits_balance + amt, updated_at = now()
  where user_id = p_user_id;

  insert into public.credit_ledger (user_id, delta, reason, batch_id, idempotency_key)
  values (p_user_id, amt, 'generation_refund', p_batch_id, 'refund:' || p_batch_id)
  on conflict (idempotency_key) do nothing;
end;
$$;

grant execute on function public.spend_generation_credit(uuid, text, integer) to service_role;
grant execute on function public.refund_generation_credit(uuid, text, integer) to service_role;
