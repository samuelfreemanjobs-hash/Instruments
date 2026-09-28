-- WO-SAAS-011: denormalized spec fields for history search/filter

alter table public.saved_kits
  add column if not exists spec_mode text,
  add column if not exists spec_engine text,
  add column if not exists spec_key text,
  add column if not exists spec_bpm integer,
  add column if not exists variation_batch_id text,
  add column if not exists variation_label text;

create index if not exists saved_kits_user_bpm_idx
  on public.saved_kits (user_id, spec_bpm, created_at desc);

create index if not exists saved_kits_user_batch_idx
  on public.saved_kits (user_id, variation_batch_id);
