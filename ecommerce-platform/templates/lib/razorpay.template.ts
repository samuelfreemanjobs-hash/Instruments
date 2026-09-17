const keyId = process.env.RAZORPAY_KEY_ID!;
const keySecret = process.env.RAZORPAY_KEY_SECRET!;

export async function createRazorpayOrder(amountPaise: number, receipt: string) {
  const auth = Buffer.from(`${keyId}:${keySecret}`).toString("base64");
  const res = await fetch("https://api.razorpay.com/v1/orders", {
    method: "POST",
    headers: {
      Authorization: `Basic ${auth}`,
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ amount: amountPaise, currency: "INR", receipt }),
  });
  if (!res.ok) throw new Error("Razorpay order failed");
  return res.json();
}
