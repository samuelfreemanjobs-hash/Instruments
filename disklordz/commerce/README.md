# Commerce v1 — simple delivery

**No complex DRM in Phase 1.** See [LICENSING.md](../docs/LICENSING.md).

## Target flow

```text
PAYMENT → ORDER → LICENSE PDF + DOWNLOAD LINK → EMAIL → CUSTOMER RECORD
```

Plugins: installer zip + personal-use license text; optional honor-system key later.

## Automation (planned)

- Gumroad / Lemon Squeezy webhook → Airtable order row  
- Workflow Automation Engineer owns integration (Sprint D in AUTOMATION_ROADMAP)

## Not in v1

Activation servers, heavy DRM, subscription billing (catalog first).
