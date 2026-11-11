import { Inngest } from "inngest";

export const inngest = new Inngest({ id: "black-friday-madness" });

export function inngestConfigured(): boolean {
  return Boolean(process.env.INNGEST_EVENT_KEY || process.env.INNGEST_SIGNING_KEY || process.env.INNGEST_DEV);
}
