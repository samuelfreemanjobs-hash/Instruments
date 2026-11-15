ALTER TABLE user_preferences
  ADD COLUMN IF NOT EXISTS budget_total NUMERIC(12, 2),
  ADD COLUMN IF NOT EXISTS budget_by_category JSONB NOT NULL DEFAULT '{}';
