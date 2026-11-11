-- Black Friday Madness — PRD Part 9 (PostgreSQL / Supabase)
-- Apply via Supabase CLI; enable RLS on user tables after auth wiring.

CREATE EXTENSION IF NOT EXISTS "pgcrypto";

-- profiles (extends auth.users)
CREATE TABLE IF NOT EXISTS profiles (
  id UUID PRIMARY KEY REFERENCES auth.users(id) ON DELETE CASCADE,
  email TEXT,
  display_name TEXT,
  avatar_url TEXT,
  created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
  updated_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE IF NOT EXISTS categories (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  parent_id UUID REFERENCES categories(id),
  name TEXT NOT NULL,
  slug TEXT NOT NULL UNIQUE,
  description TEXT,
  active BOOLEAN NOT NULL DEFAULT true
);

CREATE TABLE IF NOT EXISTS brands (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  name TEXT NOT NULL,
  slug TEXT NOT NULL UNIQUE,
  created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE IF NOT EXISTS products (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  title TEXT NOT NULL,
  brand_id UUID REFERENCES brands(id),
  category_id UUID REFERENCES categories(id),
  description TEXT,
  model_number TEXT,
  upc TEXT,
  normalized_title TEXT,
  specifications JSONB NOT NULL DEFAULT '{}',
  primary_image_url TEXT,
  created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
  updated_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE IF NOT EXISTS product_variants (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  product_id UUID NOT NULL REFERENCES products(id) ON DELETE CASCADE,
  variant_attributes JSONB NOT NULL DEFAULT '{}',
  sku TEXT,
  upc TEXT,
  created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE IF NOT EXISTS retailers (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  name TEXT NOT NULL,
  slug TEXT NOT NULL UNIQUE,
  website_url TEXT,
  logo_url TEXT,
  integration_type TEXT NOT NULL,
  active BOOLEAN NOT NULL DEFAULT true,
  created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE IF NOT EXISTS retailer_integrations (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  retailer_id UUID NOT NULL REFERENCES retailers(id) ON DELETE CASCADE,
  provider TEXT NOT NULL,
  configuration JSONB NOT NULL DEFAULT '{}',
  integration_status TEXT NOT NULL DEFAULT 'unconfigured',
  last_sync_at TIMESTAMPTZ,
  last_error TEXT,
  created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE IF NOT EXISTS product_offers (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  product_id UUID NOT NULL REFERENCES products(id) ON DELETE CASCADE,
  variant_id UUID REFERENCES product_variants(id),
  retailer_id UUID NOT NULL REFERENCES retailers(id),
  external_product_id TEXT,
  product_url TEXT NOT NULL,
  current_price NUMERIC(12, 2),
  shipping_price NUMERIC(12, 2),
  availability_status TEXT NOT NULL DEFAULT 'unknown',
  condition TEXT NOT NULL DEFAULT 'new',
  currency TEXT NOT NULL DEFAULT 'USD',
  last_verified_at TIMESTAMPTZ,
  created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
  updated_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE IF NOT EXISTS price_history (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  offer_id UUID NOT NULL REFERENCES product_offers(id) ON DELETE CASCADE,
  price NUMERIC(12, 2) NOT NULL,
  shipping_price NUMERIC(12, 2),
  recorded_at TIMESTAMPTZ NOT NULL DEFAULT now(),
  source TEXT NOT NULL,
  confidence REAL NOT NULL DEFAULT 0.8
);

CREATE TABLE IF NOT EXISTS deal_records (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  offer_id UUID NOT NULL REFERENCES product_offers(id) ON DELETE CASCADE,
  advertised_price NUMERIC(12, 2),
  current_price NUMERIC(12, 2) NOT NULL,
  deal_score INT NOT NULL CHECK (deal_score BETWEEN 0 AND 100),
  confidence REAL NOT NULL,
  classification TEXT NOT NULL,
  explanation JSONB NOT NULL DEFAULT '{}',
  verified_at TIMESTAMPTZ NOT NULL DEFAULT now(),
  expiration_at TIMESTAMPTZ,
  status TEXT NOT NULL DEFAULT 'active'
);

CREATE INDEX IF NOT EXISTS idx_offers_retailer ON product_offers(retailer_id);
CREATE INDEX IF NOT EXISTS idx_price_history_offer ON price_history(offer_id, recorded_at DESC);

-- RLS placeholders (enable per table in Supabase dashboard)
-- ALTER TABLE profiles ENABLE ROW LEVEL SECURITY;
