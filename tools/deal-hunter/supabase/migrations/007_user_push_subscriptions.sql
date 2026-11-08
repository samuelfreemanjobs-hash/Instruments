CREATE TABLE IF NOT EXISTS user_push_subscriptions (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  user_id UUID NOT NULL REFERENCES auth.users(id) ON DELETE CASCADE,
  endpoint TEXT NOT NULL,
  p256dh TEXT NOT NULL,
  auth TEXT NOT NULL,
  min_steal_score INT NOT NULL DEFAULT 85 CHECK (min_steal_score BETWEEN 50 AND 100),
  active BOOLEAN NOT NULL DEFAULT true,
  created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
  updated_at TIMESTAMPTZ NOT NULL DEFAULT now(),
  UNIQUE (user_id, endpoint)
);

ALTER TABLE user_push_subscriptions ENABLE ROW LEVEL SECURITY;

CREATE POLICY user_push_subs_select_own ON user_push_subscriptions
  FOR SELECT USING (auth.uid() = user_id);

CREATE POLICY user_push_subs_insert_own ON user_push_subscriptions
  FOR INSERT WITH CHECK (auth.uid() = user_id);

CREATE POLICY user_push_subs_update_own ON user_push_subscriptions
  FOR UPDATE USING (auth.uid() = user_id);

CREATE POLICY user_push_subs_delete_own ON user_push_subscriptions
  FOR DELETE USING (auth.uid() = user_id);

CREATE INDEX IF NOT EXISTS idx_user_push_subs_user ON user_push_subscriptions (user_id) WHERE active = true;
