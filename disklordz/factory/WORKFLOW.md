# Factory workflow — agents at every step

The **parametric factory** (API + scripts) is the machine. **Agents** are the assembly line QA and GTM — they were documented separately; this file **wires them back in**.

## Pipeline

```text
PM WO [Factory]
      │
      ▼
┌─────────────────┐
│ Cursor /api/    │  generate or factory/batch
│ generate        │
└────────┬────────┘
         │ manifest + WAVs
         ▼
┌─────────────────┐
│ artist-midnight │  lane guardian (DL002 tripwire)
│ -circuit        │
└────────┬────────┘
         ▼
┌─────────────────┐
│ ar-kit-qc       │  keep/reject list → QC_TRIPWIRE_SAMPLE.md
└────────┬────────┘
         ▼
┌─────────────────┐
│ CROW            │  pack sign-off
└────────┬────────┘
         ▼
┌─────────────────┐
│ Founder         │  legal + final ears
└────────┬────────┘
         ▼
┌─────────────────┐
│ HELM            │  launch-assets zip + Stripe live
└────────┬────────┘
         ▼
┌─────────────────┐
│ FORGE           │  email/cron + registry
└────────┬────────┘
         ▼
┌─────────────────┐
│ VARA            │  sell
└─────────────────┘
```

## Commands

| Step | Command / API |
|------|----------------|
| Batch candidates | `bash disklordz/sound-factory/scripts/batch_tripwire_sample.sh` |
| Online pack | `POST /api/factory/batch` (auth + credits) |
| QC doc | `disklordz/ar/QC_TRIPWIRE_SAMPLE.md` |
| Ship folder | `disklordz/launch-assets/tripwire-sample-v1/` |

## API hint

`POST /api/factory/batch` responses include **`agentPipeline`** — next owners after generation.

## Automation (FORGE)

When `[Factory]` WO closes in GitHub, `forge-agent-registry` + PM should attach this checklist to the WO comment.
