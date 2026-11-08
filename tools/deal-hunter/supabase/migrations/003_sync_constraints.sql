-- Sync job uniqueness for catalog upserts

CREATE UNIQUE INDEX IF NOT EXISTS idx_product_offers_retailer_external
  ON product_offers (retailer_id, external_product_id)
  WHERE external_product_id IS NOT NULL;

CREATE UNIQUE INDEX IF NOT EXISTS idx_deal_records_offer
  ON deal_records (offer_id);

CREATE INDEX IF NOT EXISTS idx_products_normalized_title ON products (normalized_title);
