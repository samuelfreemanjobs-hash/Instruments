import { Inngest } from "inngest";

export const inngest = new Inngest({
  id: "disklordz-saas",
});

export function inngestConfigured(): boolean {
  return Boolean(process.env.INNGEST_EVENT_KEY || process.env.INNGEST_DEV);
}
