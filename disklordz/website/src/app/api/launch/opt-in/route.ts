import { NextResponse } from "next/server";

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

async function notifyResend(email: string): Promise<void> {
  const apiKey = process.env.RESEND_API_KEY;
  const from = process.env.RESEND_FROM_EMAIL;
  const downloadUrl = process.env.LAUNCH_VOCAL_STEMS_URL;
  if (!apiKey || !from || !downloadUrl) return;

  await fetch("https://api.resend.com/emails", {
    method: "POST",
    headers: {
      Authorization: `Bearer ${apiKey}`,
      "Content-Type": "application/json",
    },
    body: JSON.stringify({
      from,
      to: [email],
      subject: "[DOWNLOAD] Your 1994 Memphis vocal stems",
      html: `<p>Your stem pack is ready.</p><p><a href="${downloadUrl}">Download the ZIP</a></p><p>Tip: cut a narrow notch at 3.2 kHz so your snare snaps through the vocal.</p>`,
    }),
  });
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
    await notifyResend(email);
  } catch (err) {
    console.error("[launch/opt-in] resend failed", err);
  }

  if (process.env.NODE_ENV !== "production") {
    console.info("[launch/opt-in] lead captured", { email: email.replace(/(.{2}).+(@.+)/, "$1…$2") });
  }

  return NextResponse.json({ ok: true, redirect: "/launch/thank-you" });
}
