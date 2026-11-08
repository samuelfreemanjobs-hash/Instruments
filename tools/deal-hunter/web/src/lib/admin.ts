export function isAdminEmail(email: string | undefined | null): boolean {
  if (!email) return false;
  const list = (process.env.BFM_ADMIN_EMAILS ?? process.env.ADMIN_EMAILS ?? "")
    .split(",")
    .map((s) => s.trim().toLowerCase())
    .filter(Boolean);
  if (list.length === 0) return false;
  return list.includes(email.toLowerCase());
}
