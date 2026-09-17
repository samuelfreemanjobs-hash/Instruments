import { sendEmail } from "../src/lib/email/mailer";

async function main() {
  const result = await sendEmail({
    to: "customer@example.com",
    subject: "Order confirmed — TEST1234",
    text: "Hi Demo,\n\nThanks for your order.\nWireless Headphones × 1 — $79.99\nTotal: $85.98",
    html: "<p>Hi Demo,</p><p>Thanks for your order.</p>",
  });
  console.log("sendEmail result:", result);
}

main();
