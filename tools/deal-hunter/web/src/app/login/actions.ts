"use server";

import { createClient } from "@/lib/supabase/server";
import { isSupabaseConfigured } from "@/lib/supabase/config";
import { safeRedirectPath } from "@/lib/safe-redirect";

const EMAIL_RE = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;

export async function sendMagicLink(input: { email: string; origin: string; next?: string }) {
  if (!isSupabaseConfigured()) {
    return { ok: false as const, error: "supabase_not_configured" };
  }
  const email = input.email.trim().slice(0, 320);
  if (!EMAIL_RE.test(email)) {
    return { ok: false as const, error: "invalid_email" };
  }
  const origin = input.origin.trim().slice(0, 256);
  if (!origin.startsWith("http://") && !origin.startsWith("https://")) {
    return { ok: false as const, error: "invalid_origin" };
  }
  const next = safeRedirectPath(input.next);
  const redirectTo = `${origin}/auth/callback?next=${encodeURIComponent(next)}`;

  const supabase = await createClient();
  const { error } = await supabase!.auth.signInWithOtp({
    email,
    options: {
      emailRedirectTo: redirectTo,
      shouldCreateUser: true,
    },
  });

  if (error) {
    const msg = error.message;
    if (/redirect|url/i.test(msg)) {
      return {
        ok: false as const,
        error: "redirect_not_allowed",
        detail: msg,
        hint: "Add this URL under Supabase → Authentication → URL Configuration → Redirect URLs.",
      };
    }
    if (/rate|limit|too many|exceeded/i.test(msg)) {
      return {
        ok: false as const,
        error: "rate_limited",
        detail: msg,
        hint:
          "Supabase’s shared mailer caps OTP emails (~4/hour on free tier). Wait 60 minutes, or enable Authentication → SMTP with your own provider.",
      };
    }
    return { ok: false as const, error: "send_failed", detail: msg };
  }

  return {
    ok: true as const,
    hint: "If nothing arrives in a few minutes, check spam and Supabase → Authentication → Logs.",
  };
}
