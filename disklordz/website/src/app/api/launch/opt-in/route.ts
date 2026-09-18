import { NextResponse } from "next/server";

import { processDueLaunchEmails } from "@/lib/launch/process-queue";
import { recordLeadAndSchedule } from "@/lib/launch/schedule-lead";
import { getSupabaseAdmin } from "@/lib/supabase/admin";

const EMAIL_RE = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
const OPT_IN_LIMIT = 30;
const WINDOW_MS = 24 * 60 * 60 * 1000;

type Bucket = { count: number; windowStart: number };
const buckets = new Map<string, Bucket>();

function checkOptInLimit(ip: string): boolean {
  const now = Date.now();
  let bucket = buckets.get(ip);
  if (!bucket || now - bucket.windowStart > WINDOW_MS) {
    bucket = { count: 0, windowStart: now };
    buckets.set(ip, bucket);
  }
  if (bucket.count >= OPT_IN_LIMIT) return false;
  bucket.count += 1;
  return true;
}

async function automationPath(email: string): Promise<void> {
  const supabase = getSupabaseAdmin();
  if (!supabase) return;
  const scheduled = await recordLeadAndSchedule(supabase, email);
  if ("error" in scheduled) {
    console.error("[launch/opt-in] schedule", scheduled.error);
    return;
  }
  try {
    await processDueLaunchEmails(supabase, 10);
  } catch (e) {
    console.error("[launch/opt-in] immediate queue", e);
  }
}

export async function POST(req: Request) {
  const ip =
    req.headers.get("x-forwarded-for")?.split(",")[0]?.trim() ??
    req.headers.get("x-real-ip") ??
    "unknown";

  if (!checkOptInLimit(ip)) {
    return NextResponse.json({ error: "Too many requests today." }, { status: 429 });
  }

  let body: unknown;
  try {
    body = await req.json();
  } catch {
    return NextResponse.json({ error: "Invalid JSON." }, { status: 400 });
  }

  const email =
    typeof body === "object" && body !== null && "email" in body
      ? String((body as { email: unknown }).email).trim().toLowerCase()
      : "";

  if (!email || email.length > 320 || !EMAIL_RE.test(email)) {
    return NextResponse.json({ error: "Enter a valid email." }, { status: 400 });
  }

  try {
    await automationPath(email);
  } catch (err) {
    console.error("[launch/opt-in] automation failed", err);
  }

  if (process.env.NODE_ENV !== "production") {
    console.info("[launch/opt-in] lead captured", { email: email.replace(/(.{2}).+(@.+)/, "$1…$2") });
  }

  return NextResponse.json({ ok: true, redirect: "/launch/thank-you" });
}
