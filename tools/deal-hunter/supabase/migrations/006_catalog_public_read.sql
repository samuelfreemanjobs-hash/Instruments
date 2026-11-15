-- Read-only catalog for anon/authenticated clients (Next /api/catalog/search)
ALTER TABLE products ENABLE ROW LEVEL SECURITY;
ALTER TABLE retailers ENABLE ROW LEVEL SECURITY;
ALTER TABLE product_offers ENABLE ROW LEVEL SECURITY;
ALTER TABLE deal_records ENABLE ROW LEVEL SECURITY;

DROP POLICY IF EXISTS products_public_read ON products;
CREATE POLICY products_public_read ON products FOR SELECT USING (true);

DROP POLICY IF EXISTS retailers_public_read ON retailers;
CREATE POLICY retailers_public_read ON retailers FOR SELECT USING (active = true);

DROP POLICY IF EXISTS offers_public_read ON product_offers;
CREATE POLICY offers_public_read ON product_offers FOR SELECT USING (availability_status = 'in_stock');

DROP POLICY IF EXISTS deal_records_public_read ON deal_records;
CREATE POLICY deal_records_public_read ON deal_records FOR SELECT USING (status = 'active');
