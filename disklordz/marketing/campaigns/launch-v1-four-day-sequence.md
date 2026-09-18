# Launch v1 — 4-day email sequence (DRAFT)

**Status:** Ready to use when you trigger send — **not auto-fired** in v1 ([HUMAN_TRIGGERS.md](../HUMAN_TRIGGERS.md))  
**Authored by delegation:** VARA → `mkt-lifecycle` (logic) + `mkt-email-copywriter` (copy)  
**GTM:** Day 1 free stems · **Day 2 tripwire sample** · Day 3–4 urgency  
**Replace placeholders:** `{{DOWNLOAD_URL}}`, `{{TRIPWIRE_URL}}`, `{{FLAGSHIP_URL}}`, `{{UNSUBSCRIBE}}`

---

## Day 1 — Instant value (send on opt-in)

**Trigger:** Founder / Resend after opt-in (manual or API)

**Subject:** [DOWNLOAD] Your 1994 Memphis vocal stems are inside  
**Preview:** BPM + key labeled. One EQ move inside.

**Body (plain):**

```
You asked for the vault — here it is.

→ Download your 25 cleaned, pitch-tuned Memphis phonk vocal chops: {{DOWNLOAD_URL}}

No mud. No phase war. Drop them straight on a 808.

Quick mix tip: cut a narrow notch around 3.2 kHz on the vocal bus so your snare still cuts through tape and distortion.

Tomorrow I'm opening something small for people who want the *drums* to hit the same way — not just the chops. Watch your inbox.

— Disklordz

P.S. Haven't tried the browser tape smash yet? {{LAUNCH_URL}} — flip Cassette Smash and hear the difference before you mix.

{{UNSUBSCRIBE}}
```

**CTA URL placeholders:** `LAUNCH_URL` = `https://YOUR_DOMAIN/launch`

---

## Day 2 — Tripwire sample live

**Trigger:** **Founder Day 2** — cart must be live before send

**Subject:** Day 2: Memphis phonk sample pack (lite) — $9 before I close the batch  
**Preview:** 15+ one-shots. Same vault lane as Vol. 1.

**Body (plain):**

```
Yesterday you grabbed the vocals.

Today the *knock* matches.

I pulled a **Memphis Vault Lite** tripwire — 15–20 one-shots (kicks, 808s, snares, cowbells) in the same underground lane as Memphis Vault Vol. 1.

Not a bloated 500-file pack. Just the hits you need for a dirty phonk session tonight.

→ Get the tripwire sample: {{TRIPWIRE_URL}}

Stripe checkout. Instant download. Royalty-free for your beats.

If you're still layering weak digital drums under those stems, this fixes the bottom half of the session.

— Disklordz

P.S. Full catalog heads toward Cassette Smash + the full tape plugin — lite pack buyers get first dibs on early bird pricing. {{FLAGSHIP_URL}}

{{UNSUBSCRIBE}}
```

---

## Day 3 — Origin story + proof

**Trigger:** Day 3 morning

**Subject:** Why Tommy Wright-era beats sounded violent (and your DAW doesn't)  
**Preview:** 12-bit, tape, clipping — on purpose.

**Body (plain):**

```
Clean DAWs lie to phonk producers.

The Memphis sound wasn't "high fidelity." It was **4-track mixers**, **12-bit sampling**, and **tape** eating transients until kicks turned square.

That's why I built the launch soundboard — hear **clean vs crushed** in the browser before you buy anything:

→ {{LAUNCH_URL}}

If you picked up **Memphis Vault Lite** yesterday, load a kick and a chop in the same project. Narrow band on the vocal. Let the 808 own sub under 120 Hz.

No lite pack yet? Tripwire's still here for now: {{TRIPWIRE_URL}}

— Disklordz

P.S. Tomorrow early-bird pricing on the full tape chain moves — today is for making the beat.

{{UNSUBSCRIBE}}
```

---

## Day 4 — Hard deadline

**Trigger:** Day 4 (cart close narrative — adjust date in copy when you schedule)

**Subject:** [Closing tonight] Early-bird tape access + lite pack batch  
**Preview:** Last call on launch pricing.

**Body (plain):**

```
Last call on launch week.

After tonight I'm pulling the **Memphis Vault Lite** tripwire batch from this price tier and moving focus to the full **Memphis Tape FX** early bird.

→ Lite sample pack (if still available): {{TRIPWIRE_URL}}  
→ Full tape plugin early bird: {{FLAGSHIP_URL}}

30-day no-BS refund on digital products — if it doesn't dirty your signal path, email support and we'll fix it or refund.

You already have the vocal stems. Either the drums match the energy or the track stays a podcast.

— Disklordz

P.S. Free stems again if you lost the link: {{DOWNLOAD_URL}}

{{UNSUBSCRIBE}}
```

---

## Branch notes (future automation)

| Segment | Change |
|---------|--------|
| Bought tripwire Day 2 | Day 3–4: swap tripwire CTA for Cassette Smash OTO |
| Declined all paid | Day 4: emphasize IR pack downsell URL (`LAUNCH_TRIPWIRE_IR_URL`) |

---

## PM / founder checklist before Day 1 send

- [ ] `{{DOWNLOAD_URL}}` real ZIP hosted
- [ ] Day 2: `{{TRIPWIRE_URL}}` Stripe Payment Link live
- [ ] Product name matches [TRIPWIRE_SAMPLE_PRODUCT_BRIEF.md](../products/TRIPWIRE_SAMPLE_PRODUCT_BRIEF.md)
- [ ] Resend domain verified
- [ ] You clicked send or approved schedule
