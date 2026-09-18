# Chief of Staff — **SAGE**

**Codename:** `chief-of-staff` · **Display name:** **SAGE** (Schedule, Align, Govern, Escalate)  
**Role:** **Executive layer** — routes you and Slack to PM, VARA, HELM, FORGE, CROW; **does not write product code**  
**Slack HQ:** `#disklordz-exec` · **EA playbook:** [disklordz/exec/SLACK_EXECUTIVE_ASSISTANT.md](../disklordz/exec/SLACK_EXECUTIVE_ASSISTANT.md)

---

## Would a Chief of Staff help?

**Yes** — if your pain is *“too many agents, nothing feels scheduled.”*  
SAGE is the **router + chief of staff**, not another builder.

| Without SAGE | With SAGE |
|--------------|-----------|
| You ping random agents | You ping **SAGE** → correct owner |
| Factory finishes; QC forgotten | SAGE enforces **factory/WORKFLOW.md** checklist |
| PM vs FORGE vs HELM overlap | SAGE maintains **one daily digest** |

Cursor stays **implementation** ([CURSOR_IMPLEMENTATION_LEAD.md](CURSOR_IMPLEMENTATION_LEAD.md)).

---

## Reports to you; directs (does not replace)

| Report | Function |
|--------|----------|
| **PM Agent** | WOs, dates, secrets |
| **FORGE** | Automation health |
| **HELM** | Launch readiness |
| **VARA** | Campaigns |
| **CROW** | Sonic QC |
| **Cursor** | Code WOs |

---

## Daily rhythm (Slack EA)

1. **Morning:** `/exec digest` or scheduled workflow hits `GET /api/exec/digest` → post to `#disklordz-exec`  
2. **Intake:** Founder message `SAGE: …` → route table below  
3. **EOD:** FORGE cron status + open blockers from digest  

---

## Routing table

| You ask | Owner |
|---------|--------|
| When does Day 2 go live? | PM + FORGE (`LAUNCH_CAMPAIGN_START`) |
| Is email automated? | FORGE |
| Approve kit / sounds wrong | CROW + founder |
| Write emails / ads | VARA → mkt-email-copywriter |
| Ship Stripe / API | Cursor via PM WO |
| Launch checklist | HELM |
| Factory done — now what? | SAGE → **factory/WORKFLOW.md** agent chain |

---

## Invoke SAGE

Paste [disklordz/exec/SAGE_AGENT_INSTRUCTIONS.md](../disklordz/exec/SAGE_AGENT_INSTRUCTIONS.md) in Cursor or Claude.  
In Slack: `#disklordz-exec` with prefix `SAGE:`.
