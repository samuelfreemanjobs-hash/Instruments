import nodemailer from "nodemailer";

export function isEmailConfigured() {
  return Boolean(
    process.env.SMTP_HOST?.trim() &&
      process.env.SMTP_USER?.trim() &&
      process.env.SMTP_PASS?.trim() &&
      process.env.EMAIL_FROM?.trim(),
  );
}

function createTransport() {
  const port = Number(process.env.SMTP_PORT ?? "587");
  return nodemailer.createTransport({
    host: process.env.SMTP_HOST,
    port,
    secure: port === 465,
    auth: {
      user: process.env.SMTP_USER,
      pass: process.env.SMTP_PASS,
    },
  });
}

export async function sendEmail(params: { to: string; subject: string; html: string; text: string }) {
  if (!isEmailConfigured()) {
    if (process.env.NODE_ENV !== "production") {
      console.info("[email:dev]", params.subject, "→", params.to);
      console.info(params.text);
      return { sent: false as const, devLogged: true as const };
    }
    return { sent: false as const, devLogged: false as const };
  }

  const transport = createTransport();
  await transport.sendMail({
    from: process.env.EMAIL_FROM,
    to: params.to,
    subject: params.subject,
    html: params.html,
    text: params.text,
  });
  return { sent: true as const };
}
