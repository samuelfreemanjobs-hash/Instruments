-- WO-SAAS-009: private bucket for generated kit WAVs (server uploads via service role)

insert into storage.buckets (id, name, public, file_size_limit)
values ('disklordz-kits', 'disklordz-kits', false, 5242880)
on conflict (id) do nothing;

-- No public policies: Next.js API reads/writes with SUPABASE_SERVICE_ROLE_KEY only.
